#if DEV_STAGE
#include "EditorUI.h"
#include "components/MenuBar.h"
#include "components/EntitiesWindow.h"
#include "components/ComponentDataWindow.h"
#include "components/GlobalParamsWindow.h"
#include "components/ViewportWindow.h"
#include "components/PerformanceWindow.h"
#include "components/OverviewWindow.h"
#include "renderer/ComponentRenderer.h"
#include "interfaces/WorldEditor.h"
#include "engine/Config.h"
#include "utils/Event.h"
#include <common/Logger.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/implot/implot.h>
#include <imgui/ImGuizmo/ImGuizmo.h>
#include <filesystem>

namespace editor
{

static void LoadFonts(ImGuiIO &_IO)
{
  const auto FontPath = CConfig::Instance().GetFontsDir() / "Roboto_Regular.ttf";
  if (std::filesystem::exists(FontPath))
    _IO.Fonts->AddFontFromFileTTF(FontPath.string().c_str(), 15.0f);
  else
    CLogger::Log(ELogType::Warning, "[Editor] Font file not found: %s", FontPath.string());
}

CEditorUI::CEditorUI(IWorldEditor &_WorldEditor) :
    m_WorldEditor(_WorldEditor),
    m_ComponentRenderer(std::make_unique<CComponentRenderer>()),
    m_ComponentDataWindow(std::make_unique<CComponentDataWindow>(_WorldEditor, *m_ComponentRenderer)),
    m_GlobalParamsWindow(std::make_unique<CGlobalParamsWindow>()),
    m_ViewportWindow(std::make_unique<CViewportWindow>(_WorldEditor)),
    m_MenuBar(std::make_unique<CMenuBar>()),
    m_EntitiesWindow(std::make_unique<CEntitiesWindow>(_WorldEditor)),
    m_PerformanceWindow(std::make_unique<CPerformanceWindow>()),
    m_OverviewWindow(std::make_unique<COverviewWindow>())
{
}

CEditorUI::~CEditorUI() = default;

void CEditorUI::OnEvent(const TEvent &_Event)
{
  switch (_Event.Type)
  {
  case TEventType::Editor_RequestAppClose:
    // save data if needed
    event::Notify(TEventType::RequestAppShutdown);
    break;
  }
}

void CEditorUI::SubscribeToEvents()
{
  event::Subscribe(TEventType::Editor_RequestAppClose, GetWeakPtr());
}

TVector2i CEditorUI::GetViewportSize() const
{
  return m_ViewportWindow->GetSize();
}

void CEditorUI::Init(GLFWwindow *_Window)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();

  ImGuiIO &IO     = ImGui::GetIO();
  IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  LoadFonts(IO);

  ImGui_ImplGlfw_InitForOpenGL(_Window, true);
  ImGui_ImplOpenGL3_Init("#version 460");
  ImGui::StyleColorsDark();

  SubscribeToEvents();
}

void CEditorUI::Shutdown()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();
}

void CEditorUI::RenderFrame()
{
  RenderBegin();

  m_MenuBar->Render();
  m_EntitiesWindow->Render();
  m_ComponentDataWindow->Render(m_EntitiesWindow->GetSelectedEntity());
  m_GlobalParamsWindow->Render();
  m_PerformanceWindow->Render();
  m_OverviewWindow->Render();
  m_ViewportWindow->Render(m_EntitiesWindow->GetSelectedEntity());

  RenderEnd();
}

void CEditorUI::RenderBegin()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGuizmo::BeginFrame();

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

  ImGuiID DockSpaceID = ImGui::GetID("MainDockSpace");
  ImGui::DockSpace(DockSpaceID, ImVec2(0, 0));

  static const bool InitLayout = [this, DockSpaceID, Viewport]() mutable {
    ImGui::DockBuilderRemoveNode(DockSpaceID);
    ImGui::DockBuilderAddNode(DockSpaceID, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(DockSpaceID, Viewport->WorkSize);

    ImGuiID LeftNodeID     = ImGui::DockBuilderSplitNode(DockSpaceID, ImGuiDir_Left, 0.22f, nullptr, &DockSpaceID);
    ImGuiID SceneNodeID    = ImGui::DockBuilderSplitNode(LeftNodeID, ImGuiDir_Up, 0.4f, nullptr, &LeftNodeID);
    ImGuiID RightNodeID    = ImGui::DockBuilderSplitNode(DockSpaceID, ImGuiDir_Right, 0.25f, nullptr, &DockSpaceID);
    ImGuiID RightTopNodeID = ImGui::DockBuilderSplitNode(RightNodeID, ImGuiDir_Up, 0.5f, nullptr, &RightNodeID);
    ImGuiID BotNodeID      = ImGui::DockBuilderSplitNode(DockSpaceID, ImGuiDir_Down, 0.25f, nullptr, &DockSpaceID);

    ImGui::DockBuilderDockWindow(m_EntitiesWindow->GetName().c_str(), SceneNodeID);
    ImGui::DockBuilderDockWindow(m_ComponentDataWindow->GetName().c_str(), LeftNodeID);
    ImGui::DockBuilderDockWindow(m_GlobalParamsWindow->GetName().c_str(), RightTopNodeID);
    ImGui::DockBuilderDockWindow(m_OverviewWindow->GetName().c_str(), RightNodeID);
    ImGui::DockBuilderDockWindow(m_ViewportWindow->GetName().c_str(), DockSpaceID);
    ImGui::DockBuilderDockWindow(m_PerformanceWindow->GetName().c_str(), BotNodeID);

    ImGui::DockBuilderFinish(DockSpaceID);

    return true;
  }();
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
