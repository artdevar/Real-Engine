#include "Engine.h"
#include "Config.h"
#include "Camera.h"
#include "Display.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "events/EventsManager.h"
#include "events/Event.h"
#include "editor/EditorUI.h"
#include "render/GLRenderer.h"
#include "render/RenderTypes.h"
#include "render/FrameData.h"
#include "render/RenderPipeline.h"
#include "render/RenderQueue.h"
#include "scenes/World.h"
#include <glm/glm.hpp>
#include <string>

CEngine *CEngine::Singleton = nullptr;

CEngine::CEngine()  = default;
CEngine::~CEngine() = default;

CEngine &CEngine::Instance()
{
  if (!Singleton)
    Singleton = new CEngine;

  return *Singleton;
}

void CEngine::Shutdown()
{
#if DEV_STAGE
  m_EditorUI->Shutdown();
  m_EditorUI.reset();
#endif

  m_World->Shutdown();
  m_World.reset();

  m_Camera->Shutdown();
  m_Camera.reset();

  m_ResourceManager->Shutdown();
  m_ResourceManager.reset();

  m_InputManager.reset();

  m_EventsManager->Shutdown();
  m_EventsManager.reset();

  m_Display->Shutdown();
  m_Display.reset();

  delete Singleton;
  Singleton = nullptr;
}

int CEngine::Init()
{
  m_Display         = std::make_unique<CDisplay>();
  m_InputManager    = std::make_shared<CInputManager>();
  m_EventsManager   = std::make_shared<CEventsManager>();
  m_Camera          = CCamera::Create();
  m_World           = std::make_shared<CWorld>();
  m_ResourceManager = std::make_shared<CResourceManager>();
#if DEV_STAGE
  m_EditorUI = std::make_unique<CEditorUI>();
#endif

  const std::string GameTitle       = CConfig::Instance().GetAppTitle();
  const int         DisplayInitCode = m_Display->Init(GameTitle);
  if (DisplayInitCode != EXIT_SUCCESS)
    return DisplayInitCode;

  m_Display->SetResizeCallback([this](int w, int h) {
    OnWindowResized(w, h);
  });

  m_Display->SetMouseButtonCallback([this](int b, int a, int m) {
    m_InputManager->OnMouseButton(b, a, m);
    DispatchMouseButton(b, a, m);
  });
  m_Display->SetMouseScrollCallback([this](float x, float y) {
    m_InputManager->OnMouseScroll(x, y);
  });

  m_Display->SetMouseMoveCallback([this](float x, float y) {
    m_InputManager->OnMouseMove(x, y);
    DispatchMouseMove(x, y);
  });

  m_Display->SetKeyCallback([this](int k, int a, int m) {
    m_InputManager->OnKeyEvent(k, a, m);
    DispatchKeyInput(k, a, m);
  });

#if DEV_STAGE
  m_Display->SetCursorMode(GLFW_CURSOR_NORMAL);
#else
  m_Display->SetCursorMode(GLFW_CURSOR_DISABLED);
#endif

  m_ResourceManager->Init();

  m_RenderPipeline = std::make_unique<CRenderPipeline>();
  m_RenderPipeline->Init();

  m_Camera->Init();
  m_World->Init();
#if DEV_STAGE
  m_EditorUI->Init();
#endif

  m_Camera->SetPosition(glm::vec3(0.0f, 5.0f, 20.0f));

  return EXIT_SUCCESS;
}

int CEngine::Run()
{
  std::unique_ptr<IRenderer> Renderer = std::make_unique<COpenGLRenderer>();
  Renderer->SetCamera(m_Camera);

  double LastFrameTime = 0.0;
  while (!m_Display->ShouldClose())
  {
    const double CurrentFrameTime = glfwGetTime();
    const float  FrameDelta       = static_cast<float>(CurrentFrameTime - LastFrameTime) * 1000.0f;
    LastFrameTime                 = CurrentFrameTime;

    // const int FPS = std::lround(1000.0f / FrameDelta);
    // const std::string Title = std::format("FPS={}", FPS);
    // m_Display->SetTitle(Title);

    Update(FrameDelta);
    Render(*Renderer);

    m_Display->SwapBuffers();
    m_Display->PollEvents();
  }

  return EXIT_SUCCESS;
}

void CEngine::UpdateInternal(float _TimeDelta)
{
  m_EventsManager->Update(_TimeDelta);

  m_InputManager->Update();
  ProcessInput(_TimeDelta);

  m_Camera->Update(_TimeDelta);
  m_World->Update(_TimeDelta);
}

bool CEngine::ShouldBeUpdated() const
{
  return true;
}

void CEngine::Render(IRenderer &_Renderer)
{
  CRenderQueue RenderQueue;
  TFrameData   FrameData;

  m_World->Collect(FrameData);
  m_World->Collect(RenderQueue);

  m_RenderPipeline->Render(FrameData, RenderQueue, _Renderer);

#if DEV_STAGE
  m_EditorUI->RenderFrame();
#endif
}

void CEngine::LoadConfig()
{
  // const nlohmann::json ConfigData = utils::ParseJson(ConfigPath);
  // if (ConfigData.empty())
  //   return;
  //
  // auto Iter = ConfigData.find(m_World->GetID());
  // if (Iter == ConfigData.end())
  //   return;
  //
  // m_World->ReadFromData(Iter.value());
}

void CEngine::SaveConfig()
{
  // nlohmann::json Data;
  // Data[m_World->GetID()] = m_World->GetSaveData();
  //
  // utils::SaveJson(ConfigPath, Data);
}

TVector2i CEngine::GetWindowSize() const
{
  return m_Display->GetSize();
}

CDisplay *CEngine::GetDisplay() const
{
  return m_Display.get();
}

std::shared_ptr<CWorld> CEngine::GetWorld() const
{
  return m_World;
}

std::shared_ptr<CCamera> CEngine::GetCamera() const
{
  return m_Camera;
}

std::shared_ptr<CResourceManager> CEngine::GetResourceManager() const
{
  return m_ResourceManager;
}

std::shared_ptr<CInputManager> CEngine::GetInputManager() const
{
  return m_InputManager;
}

std::shared_ptr<CEventsManager> CEngine::GetEventsManager() const
{
  return m_EventsManager;
}

// Callbacks

void CEngine::OnWindowResized(int _Width, int _Height)
{
}

void CEngine::ProcessInput(float _TimeDelta)
{
#if !DEV_STAGE
  if (m_InputManager->IsKeyJustPressed(GLFW_KEY_ESCAPE))
    m_Display->SetShouldClose(true);
#endif
}

void CEngine::DispatchKeyInput(int _Key, int _Action, int _Mods)
{
  if (m_Camera)
    m_Camera->ProcessKeyInput(_Key, _Action, _Mods);
}

void CEngine::DispatchMouseButton(int _Button, int _Action, int _Mods)
{
#if DEV_STAGE
  if (_Button == GLFW_MOUSE_BUTTON_RIGHT)
  {
    if (_Action == GLFW_PRESS)
    {
      m_CameraDragActive = true;
    }
    else if (_Action == GLFW_RELEASE)
    {
      m_CameraDragActive = false;
      m_Camera->ResetInputState();
    }
  }
#endif

  if (m_Camera)
    m_Camera->OnMousePressed(_Button, _Action, _Mods);
}

void CEngine::DispatchMouseMove(float _X, float _Y)
{
  static bool  firstMouse = true;
  static float lastX      = 0.0f;
  static float lastY      = 0.0f;

#if DEV_STAGE
  if (!m_CameraDragActive)
  {
    firstMouse = true;
    return;
  }
#endif

  if (firstMouse)
  {
    lastX      = _X;
    lastY      = _Y;
    firstMouse = false;
    return;
  }

  float deltaX = _X - lastX;
  float deltaY = lastY - _Y;
  lastX        = _X;
  lastY        = _Y;

  if (m_Camera)
    m_Camera->ProcessMouseMove(deltaX, deltaY);
}
