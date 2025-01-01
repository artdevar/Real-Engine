#include "Engine.h"
#include "Camera.h"
#include "ResourceManager.h"
#include "World.h"
#include "graphics/Renderer.h"
#include "graphics/Model.h"
#include "graphics/Shader.h"
#include "ui/EditorUI.h"
#include "utils/Json.h"
#include "utils/Logger.h"
#include "utils/Stopwatch.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <nlohmann/json.hpp>
#include <format>
#include <string>
#include <fstream>

const std::string CEngine::ConfigPath = "configs/data.json";
CEngine * CEngine::Singleton = nullptr;

CEngine::CEngine() :
  m_Window(nullptr),
  m_Camera(nullptr),
  m_IsEditorMode(false),
  m_PrevCursorPosX(0.0),
  m_PrevCursorPosY(0.0)
{
  m_ResourceManager = std::make_shared<CResourceManager>();
  m_World           = new CWorld;
  m_Camera          = std::make_shared<CCamera>();
  m_EditorUI        = new CEditorUI;
}

CEngine::~CEngine() = default;

CEngine * CEngine::Instance()
{
  if (!Singleton)
    Singleton = new CEngine;

  return Singleton;
}

void CEngine::Shutdown()
{
  m_EditorUI->Shutdown();
  delete m_EditorUI;

  m_World->Shutdown();
  delete m_World;

  m_Camera.reset();

  m_ResourceManager->Shutdown();
  m_ResourceManager.reset();

  glfwDestroyWindow(m_Window);
  glfwTerminate();

  m_Window = nullptr;

  delete Singleton;
}

int CEngine::Init()
{
  CLogger::Log(ELogType::Info, "Init GLFW. Version: {}\n", glfwGetVersionString());
  if (glfwInit() != GLFW_TRUE)
  {
    const char * ErrorDescription;
    const int    ErrorCode = glfwGetError(&ErrorDescription);

    CLogger::Log(ELogType::Error, "Init GLFW error: {}\n", ErrorDescription);

    return ErrorCode;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  m_Window = glfwCreateWindow(WindowWidth, WindowHeight, "Game", nullptr, nullptr);
  if (!m_Window)
  {
    const char * ErrorDescription;
    const int    ErrorCode = glfwGetError(&ErrorDescription);

    CLogger::Log(ELogType::Error, "GLFW window creation error: {}\n", ErrorDescription);

    return ErrorCode;
  }

  glfwMakeContextCurrent(m_Window);

  if (!gladLoadGL())
  {
    const int ErrorCode = glad_glGetError();
    CLogger::Log(ELogType::Error, "glad load gl failed: {}\n", ErrorCode);
    return ErrorCode;
  }

  if (!GLAD_GL_ARB_bindless_texture)
  {
    CLogger::Log(ELogType::Error, "ARB_bindless_texture isn't supported by the GPU\n");
    //return EXIT_FAILURE;
  }

  glViewport(0, 0, WindowWidth, WindowHeight);
  glfwSwapInterval(1); // VSYNC
  glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_BLEND);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  InitCallbacks();

  m_ResourceManager->Init();
  m_World->Init();
  m_EditorUI->Init(this);

  return EXIT_SUCCESS;
}

int CEngine::Run()
{
  CRenderer Renderer;
  Renderer.SetCamera(m_Camera);

  m_Camera->SetPosition(glm::vec3(0.0f, 5.0f, 20.0f));

  double LastFrameTime = 0.0;
  while (!glfwWindowShouldClose(m_Window))
  {
    if (GLenum Error = glGetError(); Error != GL_NO_ERROR)
      OnGLErrorOccured(Error);

    const auto WindowSize = GetWindowSize();

    Renderer.SetViewport(WindowSize.x, WindowSize.y);
    Renderer.ClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    Renderer.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const double CurrentFrameTime = glfwGetTime();
    const float FrameDelta = static_cast<float>(CurrentFrameTime - LastFrameTime) * 1000.0f;
    LastFrameTime = CurrentFrameTime;

    const int FPS = std::lround(1000.0f / FrameDelta);
    const std::string Title = std::format("FPS={}", FPS);
    glfwSetWindowTitle(m_Window, Title.c_str());

    {
      m_Camera->Update(FrameDelta);
      m_World->Update(FrameDelta);
      if (m_IsEditorMode)
        m_EditorUI->Update(FrameDelta);
    }

    {
      m_World->Render(Renderer);
      if (m_IsEditorMode)
        m_EditorUI->Render(Renderer);
    }

    glfwSwapBuffers(m_Window);
    glfwPollEvents();
  }

  return EXIT_SUCCESS;
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
  glm::ivec2 Size;
  glfwGetWindowSize(m_Window, &Size.x, &Size.y);
  return Size;
}

CWorld * CEngine::GetWorld() const
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

void CEngine::InitCallbacks()
{
  glfwSetWindowUserPointer(m_Window, this);

  constexpr auto OnWindowResizedProxy = [](GLFWwindow * window, int width, int height)
  {
    static_cast<CEngine *>(glfwGetWindowUserPointer(window))->OnWindowResized(width, height);
  };

  constexpr auto OnMousePressedProxy = [](GLFWwindow * window, int button, int action, int mods)
  {
    static_cast<CEngine *>(glfwGetWindowUserPointer(window))->OnMousePressed(button, action, mods);
  };

  constexpr auto OnMouseScrollProxy = [](GLFWwindow * window, double xoffset, double yoffset)
  {
    static_cast<CEngine *>(glfwGetWindowUserPointer(window))->OnMouseScroll(static_cast<float>(xoffset), static_cast<float>(yoffset));
  };

  constexpr auto ProcessKeyInputProxy = [](GLFWwindow * window, int key, int scancode, int action, int mods)
  {
    static_cast<CEngine *>(glfwGetWindowUserPointer(window))->ProcessKeyInput(key, action, mods);
  };

  constexpr auto ProcessMouseMoveProxy = [](GLFWwindow * window, double xpos, double ypos)
  {
    static_cast<CEngine *>(glfwGetWindowUserPointer(window))->ProcessMouseMove(xpos, ypos);
  };

  constexpr auto OnErrorOccuredProxy = [](int error_code, const char * description)
  {
    CLogger::Log(ELogType::Error, std::format("GLFW error occured. Code: {}.\nDescription: {}.\n", error_code, description));
  };

  glfwSetFramebufferSizeCallback(m_Window, OnWindowResizedProxy);
  glfwSetMouseButtonCallback(m_Window, OnMousePressedProxy);
  glfwSetCursorPosCallback(m_Window, ProcessMouseMoveProxy);
  glfwSetScrollCallback(m_Window, OnMouseScrollProxy);
  glfwSetKeyCallback(m_Window, ProcessKeyInputProxy);
  glfwSetErrorCallback(OnErrorOccuredProxy);
}

// Callbacks

void CEngine::OnWindowResized(int _Width, int _Height)
{
  glViewport(0, 0, _Width, _Height);
}

void CEngine::OnMousePressed(int _Button, int _Action, int _Mods)
{
  if (m_IsEditorMode)
    return;

  m_Camera->OnMousePressed(_Button, _Action, _Mods);
}

void CEngine::OnMouseScroll(float _OffsetX, float _OffsetY)
{
  if (m_IsEditorMode)
    return;
}

void CEngine::ProcessMouseMove(float _X, float _Y)
{
  if (m_IsEditorMode)
    return;

  bool Handled = false;
  Handled = m_Camera->ProcessMouseMove(_X, _Y);
}

void CEngine::ProcessKeyInput(int _Key, int _Action, int _Mods)
{
  if (_Key == GLFW_KEY_F2 && _Action == GLFW_PRESS)
    EnableDebugMode(!m_IsEditorMode);

  if (m_IsEditorMode)
    return;

  bool Handled = false;
  Handled = m_Camera->ProcessKeyInput(_Key, _Action, _Mods);

  if (Handled)
    return;

  glfwSetWindowShouldClose(m_Window, _Key == GLFW_KEY_ESCAPE && _Action == GLFW_PRESS);
}

void CEngine::EnableDebugMode(bool _Enable)
{
  m_IsEditorMode = _Enable;

  if (m_IsEditorMode)
  {
    glfwGetCursorPos(m_Window, &m_PrevCursorPosX, &m_PrevCursorPosY);
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
  else
  {
    glfwSetCursorPos(m_Window, m_PrevCursorPosX, m_PrevCursorPosY);
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
}

void CEngine::OnGLErrorOccured(GLenum _Error)
{
  auto GetErrorDescription = [](GLenum _Error) -> std::string_view
  {
    switch (_Error)
    {
      case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";

      case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";

      case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";

      case GL_STACK_OVERFLOW:
        return "GL_STACK_OVERFLOW";

      case GL_STACK_UNDERFLOW:
        return "GL_STACK_UNDERFLOW";

      case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";

      case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";

      default:
        return "UNDEFINED";
    }
  };

  CLogger::Log(ELogType::Error, "OpenGL: {} (0x{:x})\n", GetErrorDescription(_Error), _Error);
}
