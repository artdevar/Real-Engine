#include "Display.h"
#include "utils/Logger.h"
#include <format>
#include <cstdlib>
#include <cstring>

static inline void ApplyInitHints()
{
#if defined(__linux__)
    // Force X11 only on Linux (needed for apitrace / GLX)
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif
}

CDisplay::CDisplay() : m_Window(nullptr)
{
}

CDisplay::~CDisplay() = default;

int CDisplay::Init(const std::string &_Title)
{
    ApplyInitHints();

    CLogger::Log(ELogType::Info, "[CDisplay] Init GLFW. Version: {}", glfwGetVersionString());
    if (glfwInit() != GLFW_TRUE)
    {
        const char *ErrorDescription;
        const int ErrorCode = glfwGetError(&ErrorDescription);
        CLogger::Log(ELogType::Fatal, "[CDisplay] Init GLFW error: {}", ErrorDescription);
        return ErrorCode;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 16);
    // glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);

    m_Window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, _Title.c_str(), nullptr, nullptr);
    if (!m_Window)
    {
        const char *ErrorDescription;
        const int ErrorCode = glfwGetError(&ErrorDescription);
        CLogger::Log(ELogType::Fatal, "[CDisplay] GLFW window creation error: {}", ErrorDescription);
        return ErrorCode;
    }

    glfwMakeContextCurrent(m_Window);

    if (!gladLoadGL())
    {
        const int ErrorCode = glad_glGetError();
        CLogger::Log(ELogType::Fatal, "[CDisplay] glad load gl failed: {}", ErrorCode);
        return ErrorCode;
    }

    if (!GLAD_GL_ARB_bindless_texture)
    {
        CLogger::Log(ELogType::Warning, "[CDisplay] ARB_bindless_texture isn't supported by the GPU");
    }

    CLogger::Log(ELogType::Info, "[CDisplay] OpenGL Version: {}. Vendor: {}. Renderer: {}", GLVersion.major, GLVersion.minor, reinterpret_cast<const char *>(glGetString(GL_VERSION)), reinterpret_cast<const char *>(glGetString(GL_VENDOR)), reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
    CLogger::Log(ELogType::Info, "[CDisplay] GLSL version: {}", reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
    {
        int MaxTextureUnits = 0;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MaxTextureUnits);
        CLogger::Log(ELogType::Info, "[CDisplay] Maximum supported texture image units: {}", MaxTextureUnits);
    }

    glViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    glfwSwapInterval(1); // VSYNC
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_MULTISAMPLE);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    InitCallbacks();

    return EXIT_SUCCESS;
}

void CDisplay::Shutdown()
{
    if (m_Window)
    {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }
    glfwTerminate();
}

void CDisplay::PollEvents()
{
    glfwPollEvents();
}

void CDisplay::SwapBuffers()
{
    if (m_Window)
        glfwSwapBuffers(m_Window);
}

bool CDisplay::ShouldClose() const
{
    return m_Window ? glfwWindowShouldClose(m_Window) : true;
}

void CDisplay::SetShouldClose(bool _ShouldClose)
{
    if (m_Window)
        glfwSetWindowShouldClose(m_Window, _ShouldClose);
}

void CDisplay::SetTitle(const std::string &_Title)
{
    if (m_Window)
        glfwSetWindowTitle(m_Window, _Title.c_str());
}

glm::ivec2 CDisplay::GetSize() const
{
    glm::ivec2 Size;
    if (m_Window)
        glfwGetWindowSize(m_Window, &Size.x, &Size.y);
    return Size;
}

GLFWwindow *CDisplay::GetWindow() const
{
    return m_Window;
}

void CDisplay::SetCursorMode(int _Mode)
{
    if (m_Window)
        glfwSetInputMode(m_Window, GLFW_CURSOR, _Mode);
}

void CDisplay::GetCursorPos(double &_X, double &_Y) const
{
    if (m_Window)
        glfwGetCursorPos(m_Window, &_X, &_Y);
}

void CDisplay::SetCursorPos(double _X, double _Y)
{
    if (m_Window)
        glfwSetCursorPos(m_Window, _X, _Y);
}

void CDisplay::SetResizeCallback(ResizeCallback _Callback)
{
    m_ResizeCallback = _Callback;
}

void CDisplay::SetMouseButtonCallback(MouseButtonCallback _Callback)
{
    m_MouseButtonCallback = _Callback;
}

void CDisplay::SetMouseScrollCallback(MouseScrollCallback _Callback)
{
    m_MouseScrollCallback = _Callback;
}

void CDisplay::SetMouseMoveCallback(MouseMoveCallback _Callback)
{
    m_MouseMoveCallback = _Callback;
}

void CDisplay::SetKeyCallback(KeyCallback _Callback)
{
    m_KeyCallback = _Callback;
}

void CDisplay::InitCallbacks()
{
    glfwSetWindowUserPointer(m_Window, this);
    glfwSetFramebufferSizeCallback(m_Window, OnWindowResizedProxy);
    glfwSetMouseButtonCallback(m_Window, OnMousePressedProxy);
    glfwSetCursorPosCallback(m_Window, OnMouseMoveProxy);
    glfwSetScrollCallback(m_Window, OnMouseScrollProxy);
    glfwSetKeyCallback(m_Window, OnKeyPressedProxy);
    glfwSetErrorCallback(OnErrorOccuredProxy);
}

void CDisplay::OnWindowResizedProxy(GLFWwindow *_Window, int _Width, int _Height)
{
    auto *Display = static_cast<CDisplay *>(glfwGetWindowUserPointer(_Window));
    if (Display)
    {
        glViewport(0, 0, _Width, _Height);
        if (Display->m_ResizeCallback)
            Display->m_ResizeCallback(_Width, _Height);
    }
}

void CDisplay::OnMousePressedProxy(GLFWwindow *_Window, int _Button, int _Action, int _Mods)
{
    auto *Display = static_cast<CDisplay *>(glfwGetWindowUserPointer(_Window));
    if (Display && Display->m_MouseButtonCallback)
        Display->m_MouseButtonCallback(_Button, _Action, _Mods);
}

void CDisplay::OnMouseScrollProxy(GLFWwindow *_Window, double _OffsetX, double _OffsetY)
{
    auto *Display = static_cast<CDisplay *>(glfwGetWindowUserPointer(_Window));
    if (Display && Display->m_MouseScrollCallback)
        Display->m_MouseScrollCallback(static_cast<float>(_OffsetX), static_cast<float>(_OffsetY));
}

void CDisplay::OnMouseMoveProxy(GLFWwindow *_Window, double _PosX, double _PosY)
{
    auto *Display = static_cast<CDisplay *>(glfwGetWindowUserPointer(_Window));
    if (Display && Display->m_MouseMoveCallback)
        Display->m_MouseMoveCallback(static_cast<float>(_PosX), static_cast<float>(_PosY));
}

void CDisplay::OnKeyPressedProxy(GLFWwindow *_Window, int _Key, int _Scancode, int _Action, int _Mods)
{
    auto *Display = static_cast<CDisplay *>(glfwGetWindowUserPointer(_Window));
    if (Display && Display->m_KeyCallback)
        Display->m_KeyCallback(_Key, _Action, _Mods);
}

void CDisplay::OnErrorOccuredProxy(int _ErrorCode, const char *_Description)
{
    CLogger::Log(ELogType::Error, std::format("GLFW error occured. Code: {}.\nDescription: {}.\n", _ErrorCode, _Description));
}
