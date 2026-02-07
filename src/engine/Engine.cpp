#include "Engine.h"
#include "Display.h"
#include "InputManager.h"
#include "Camera.h"
#include "ResourceManager.h"
#include "scenes/World.h"
#include "graphics/Renderer.h"
#include "editor/EditorUI.h"
#include <glm/glm.hpp>
#include <string>

CEngine *CEngine::Singleton = nullptr;

CEngine::CEngine() : m_Display(std::make_unique<CDisplay>()),
                     m_InputManager(std::make_shared<CInputManager>()),
                     m_ResourceManager(std::make_shared<CResourceManager>()),
                     m_World(std::make_shared<CWorld>()),
                     m_Camera(std::make_shared<CCamera>())
{
#if ENABLE_EDITOR
  m_EditorUI = new CEditorUI;
#endif
}

CEngine::~CEngine() = default;

CEngine &CEngine::Instance()
{
  if (!Singleton)
    Singleton = new CEngine;

  return *Singleton;
}

void CEngine::Shutdown()
{
#if ENABLE_EDITOR
  m_EditorUI->Shutdown();
  delete m_EditorUI;
#endif

  m_World->Shutdown();
  m_World.reset();

  m_Camera.reset();

  m_ResourceManager->Shutdown();
  m_ResourceManager.reset();

  m_InputManager.reset();

  m_Display->Shutdown();
  m_Display.reset();

  delete Singleton;
}

int CEngine::Init(const std::string &_ConfigPath, const std::string &_GameTitle)
{
  const int DisplayInitCode = m_Display->Init(_GameTitle);
  if (DisplayInitCode != EXIT_SUCCESS)
    return DisplayInitCode;

  m_Display->SetResizeCallback([this](int w, int h)
                               { OnWindowResized(w, h); });

  m_Display->SetMouseButtonCallback([this](int b, int a, int m)
                                    {
                                      m_InputManager->OnMouseButton(b, a, m);
                                      DispatchMouseButton(b, a, m); });
  m_Display->SetMouseScrollCallback([this](float x, float y)
                                    { m_InputManager->OnMouseScroll(x, y); });

  m_Display->SetMouseMoveCallback([this](float x, float y)
                                  {
                                    m_InputManager->OnMouseMove(x, y);
                                    DispatchMouseMove(x, y); });

  m_Display->SetKeyCallback([this](int k, int a, int m)
                            {
                              m_InputManager->OnKeyEvent(k, a, m);
                              DispatchKeyInput(k, a, m); });

#if ENABLE_EDITOR
  m_Display->SetCursorMode(GLFW_CURSOR_NORMAL);
#else
  m_Display->SetCursorMode(GLFW_CURSOR_DISABLED);
#endif

  m_ResourceManager->Init();
  m_World->Init();
#if ENABLE_EDITOR
  m_EditorUI->Init(this);
#endif

  return EXIT_SUCCESS;
}

int CEngine::Run()
{
  CRenderer Renderer;
  Renderer.SetCamera(m_Camera);

  m_Camera->SetPosition(glm::vec3(0.0f, 5.0f, 20.0f));

  double LastFrameTime = 0.0;
  while (!m_Display->ShouldClose())
  {
    const glm::ivec2 WindowSize = GetWindowSize();

    Renderer.SetViewport(WindowSize.x, WindowSize.y);
    Renderer.BeginFrame(0.86f, 0.86f, 0.86f, 1.0f);

    const double CurrentFrameTime = glfwGetTime();
    const float FrameDelta = static_cast<float>(CurrentFrameTime - LastFrameTime) * 1000.0f;
    LastFrameTime = CurrentFrameTime;

    // const int FPS = std::lround(1000.0f / FrameDelta);
    // const std::string Title = std::format("FPS={}", FPS);
    // m_Display->SetTitle(Title);

    Update(FrameDelta);
    Render(Renderer);

    Renderer.EndFrame();
    m_Display->SwapBuffers();
    m_Display->PollEvents();
  }

  return EXIT_SUCCESS;
}

void CEngine::Update(float _TimeDelta)
{
  m_InputManager->Update();
  ProcessInput(_TimeDelta);

  m_Camera->Update(_TimeDelta);
  m_World->Update(_TimeDelta);
#if ENABLE_EDITOR
  m_EditorUI->Update(_TimeDelta);
#endif
}

void CEngine::Render(CRenderer &_Renderer)
{
  m_World->Render(_Renderer);
#if ENABLE_EDITOR
  m_EditorUI->Render(_Renderer);
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

glm::ivec2 CEngine::GetWindowSize() const
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

// Callbacks

void CEngine::OnWindowResized(int _Width, int _Height)
{
}

void CEngine::ProcessInput(float _TimeDelta)
{
#if !ENABLE_EDITOR
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
#if ENABLE_EDITOR
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
  static bool firstMouse = true;
  static float lastX = 0.0f;
  static float lastY = 0.0f;

#if ENABLE_EDITOR
  if (!m_CameraDragActive)
  {
    firstMouse = true;
    return;
  }
#endif

  if (firstMouse)
  {
    lastX = _X;
    lastY = _Y;
    firstMouse = false;
    return;
  }

  float deltaX = _X - lastX;
  float deltaY = lastY - _Y;
  lastX = _X;
  lastY = _Y;

  if (m_Camera)
    m_Camera->ProcessMouseMove(deltaX, deltaY);
}
