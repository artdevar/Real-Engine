#if DEV_STAGE
#include "EditorUI.h"
#include "components/MenuBar.h"
#include "components/SceneWindow.h"
#include "components/ComponentDataWindow.h"
#include "components/GlobalParamsWindow.h"
#include "components/ViewportWindow.h"
#include "renderer/ComponentRenderer.h"
#include "interfaces/WorldEditor.h"
#include "engine/Config.h"
#include "utils/Logger.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <filesystem>

namespace editor
{

static void LoadFonts(ImGuiIO &_IO)
{
  const auto FontPath = CConfig::Instance().GetFontsDir() / "Roboto_Regular.ttf";
  if (std::filesystem::exists(FontPath))
    _IO.Fonts->AddFontFromFileTTF(FontPath.string().c_str(), 14.0f);
  else
    CLogger::Log(ELogType::Warning, "[Editor] Font file not found: %s", FontPath.string());
}

CEditorUI::CEditorUI(IWorldEditor &_WorldEditor) :
    m_WorldEditor(_WorldEditor),
    m_ComponentRenderer(std::make_unique<CComponentRenderer>()),
    m_ComponentDataWindow(std::make_unique<CComponentDataWindow>(_WorldEditor, *m_ComponentRenderer)),
    m_GlobalParamsWindow(std::make_unique<CGlobalParamsWindow>()),
    m_ViewportWindow(std::make_unique<CViewportWindow>()),
    m_MenuBar(std::make_unique<CMenuBar>()),
    m_SceneWindow(std::make_unique<CSceneWindow>(_WorldEditor))
{
}

CEditorUI::~CEditorUI() = default;

TVector2i CEditorUI::GetViewportSize() const
{
  return m_ViewportWindow->GetSize();
}

void CEditorUI::Init(GLFWwindow *_Window)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &IO     = ImGui::GetIO();
  IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  LoadFonts(IO);

  ImGui_ImplGlfw_InitForOpenGL(_Window, true);
  ImGui_ImplOpenGL3_Init("#version 460");
  ImGui::StyleColorsDark();
}

void CEditorUI::Shutdown()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void CEditorUI::RenderFrame()
{
  RenderBegin();

  m_MenuBar->Render();
  m_ViewportWindow->Render();
  m_SceneWindow->Render();
  m_ComponentDataWindow->Render(m_SceneWindow->GetSelectedEntity());
  m_GlobalParamsWindow->Render();

  RenderEnd();
}

void CEditorUI::RenderBegin()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGuiViewport *Viewport = ImGui::GetMainViewport();

  ImGui::SetNextWindowPos(Viewport->WorkPos);
  ImGui::SetNextWindowSize(Viewport->WorkSize);
  ImGui::SetNextWindowViewport(Viewport->ID);

  constexpr ImGuiWindowFlags Flags = ImGuiWindowFlags_NoTitleBar |            //
                                     ImGuiWindowFlags_NoCollapse |            //
                                     ImGuiWindowFlags_NoResize |              //
                                     ImGuiWindowFlags_NoMove |                //
                                     ImGuiWindowFlags_NoBringToFrontOnFocus | //
                                     ImGuiWindowFlags_NoNavFocus |            //
                                     ImGuiWindowFlags_NoDocking;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));

  ImGui::Begin("DockSpaceRoot", nullptr, Flags);

  ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
  ImGui::DockSpace(dockspace_id, ImVec2(0, 0));

  static bool layout_initialized = false;
  if (!layout_initialized)
  {
    layout_initialized = true;

    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, Viewport->WorkSize);

    ImGuiID left_node  = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.25f, nullptr, &dockspace_id);
    ImGuiID scene_node = ImGui::DockBuilderSplitNode(left_node, ImGuiDir_Up, 0.5f, nullptr, &left_node);
    ImGuiID right_node = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr, &dockspace_id);

    ImGui::DockBuilderDockWindow(m_SceneWindow->GetName().c_str(), scene_node);
    ImGui::DockBuilderDockWindow(m_ComponentDataWindow->GetName().c_str(), left_node);
    ImGui::DockBuilderDockWindow(m_GlobalParamsWindow->GetName().c_str(), right_node);
    ImGui::DockBuilderDockWindow(m_ViewportWindow->GetName().c_str(), dockspace_id);

    ImGui::DockBuilderFinish(dockspace_id);
  }
}

void CEditorUI::RenderEnd()
{
  ImGui::End();
  ImGui::PopStyleVar();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  ImGui::EndFrame();
}

} // namespace editor

#endif
