#include "EditorUI.h"
#include "pch.h"

#if DEV_STAGE
#include "engine/Config.h"
#include "engine/Engine.h"
#include "engine/Display.h"
#include "engine/Camera.h"
#include "interfaces/WorldEditor.h"
#include "platform/SysUtils.h"
#include "utils/Resource.h"
#include "ecs/Components.h"
#include "ecs/ComponentsFactory.h"
#include <ecs/EntitySpawner.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace editor
{

CEditorUI::CEditorUI(IWorldEditor &_WorldEditor) :
    m_WorldEditor(_WorldEditor)
{
}

CEditorUI::~CEditorUI() = default;

void CEditorUI::Init()
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(CEngine::Instance().GetDisplay()->GetWindow(), true);
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

  ImGui::Begin("Main window##MainWindow");

  RenderEntities();

  RenderEnd();
}

void CEditorUI::RenderBegin()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void CEditorUI::RenderEnd()
{
  ImGui::End();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  ImGui::EndFrame();
}

void CEditorUI::RenderGlobalParams()
{
  if (ImGui::CollapsingHeader("Render"))
  {
    bool ShadowsEnabled = CConfig::Instance().GetShadowsEnabled();
    if (ImGui::Checkbox("Shadows enabled", &ShadowsEnabled))
      CConfig::Instance().SetShadowsEnabled(ShadowsEnabled, CPasskey(this));

    int ShadowMapSize = CConfig::Instance().GetShadowMapSize();
    if (ImGui::DragInt("Shadow map size", &ShadowMapSize, 256, 512, 4096))
      CConfig::Instance().SetShadowsMapSize(ShadowMapSize, CPasskey(this));

    bool FXAAEnabled = CConfig::Instance().GetFXAAEnabled();
    if (ImGui::Checkbox("FXAA enabled", &FXAAEnabled))
      CConfig::Instance().SetFXAAEnabled(FXAAEnabled, CPasskey(this));

    bool HDREnabled = CConfig::Instance().GetHDREnabled();
    if (ImGui::Checkbox("HDR enabled", &HDREnabled))
      CConfig::Instance().SetHDREnabled(HDREnabled, CPasskey(this));

    float HDRExposure = CConfig::Instance().GetHDRExposure();
    if (ImGui::DragFloat("HDR exposure", &HDRExposure, 0.1f, 0.0f, 10.0f))
      CConfig::Instance().SetHDRExposure(HDRExposure, CPasskey(this));
  }

  if (ImGui::CollapsingHeader("Camera"))
  {
    float CameraZNear = CConfig::Instance().GetCameraZNear();
    if (ImGui::DragFloat("Camera ZNear", &CameraZNear, 0.5f, -50.0f, 100.0f))
      CConfig::Instance().SetCameraZNear(CameraZNear, CPasskey(this));

    float CameraZFar = CConfig::Instance().GetCameraZFar();
    if (ImGui::DragFloat("Camera ZFar", &CameraZFar, 10.0f, 1.0f, 10000.0f))
      CConfig::Instance().SetCameraZFar(CameraZFar, CPasskey(this));

    float CameraFOV = CConfig::Instance().GetCameraFOV();
    if (ImGui::DragFloat("Camera FOV", &CameraFOV, 1.0f, 0.0f, 180.0f))
      CConfig::Instance().SetCameraFOV(CameraFOV, CPasskey(this));
  }

  if (ImGui::CollapsingHeader("Light"))
  {
    float LightZNear = CConfig::Instance().GetLightSpaceMatrixZNear();
    if (ImGui::DragFloat("Light ZNear", &LightZNear, 0.5f, -50.0f, 100.0f))
      CConfig::Instance().SetLightSpaceMatrixZNear(LightZNear, CPasskey(this));

    float LightZFar = CConfig::Instance().GetLightSpaceMatrixZFar();
    if (ImGui::DragFloat("Light ZFar", &LightZFar, 10.0f, 1.0f, 1000.0f))
      CConfig::Instance().SetLightSpaceMatrixZFar(LightZFar, CPasskey(this));

    float LightOrthLeftBot = CConfig::Instance().GetLightSpaceMatrixOrthLeftBot();
    if (ImGui::DragFloat("Light Orth Left/Bottom", &LightOrthLeftBot, 1.0f, -100.0f, 0.0f))
      CConfig::Instance().SetLightSpaceMatrixOrthLeftBot(LightOrthLeftBot, CPasskey(this));

    float LightOrthRightTop = CConfig::Instance().GetLightSpaceMatrixOrthRightTop();
    if (ImGui::DragFloat("Light Orth Right/Top", &LightOrthRightTop, 1.0f, 0.0f, 100.0f))
      CConfig::Instance().SetLightSpaceMatrixOrthRightTop(LightOrthRightTop, CPasskey(this));
  }
}

void CEditorUI::RenderEntities()
{
  if (ImGui::CollapsingHeader("Entities##EntitiesWindow"))
  {
    static bool IsPopupOpen = true;
    if (ImGui::Button("Add##AddEntity"))
    {
      ImGui::OpenPopup("Spawn entity");
      IsPopupOpen = true;
    }

    if (ImGui::BeginPopupModal("Spawn entity", &IsPopupOpen, ImGuiWindowFlags_AlwaysAutoResize))
    {
      constexpr std::pair<const char *, ecs::TEntityType> EntityTypes[] = {std::make_pair("Static mesh", ecs::TEntityType::StaticMesh),
                                                                           // std::make_pair("Point light", ecs::TEntityType::PointLight),
                                                                           std::make_pair("Directional light", ecs::TEntityType::DirectionalLight),
                                                                           // std::make_pair("Spotlight", ecs::TEntityType::Spotlight),
                                                                           std::make_pair("Skybox", ecs::TEntityType::Skybox)};

      static int CurrentIndex = -1;

      if (ImGui::BeginListBox("Select entity type"))
      {
        for (int i = 0; i < IM_ARRAYSIZE(EntityTypes); ++i)
        {
          const bool IsSelected = (CurrentIndex == i);
          if (ImGui::Selectable(EntityTypes[i].first, IsSelected))
            CurrentIndex = i;

          if (IsSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
      }

      if (CurrentIndex != -1)
      {
        if (ImGui::Button("Spawn"))
        {
          SpawnEntity(EntityTypes[CurrentIndex].second);
          ImGui::CloseCurrentPopup();
        }
      }

      if (!IsPopupOpen)
        CurrentIndex = -1;

      ImGui::EndPopup();
    }

    if (m_SelectedEntity.has_value())
    {
      ImGui::SameLine();
      if (ImGui::Button("Delete##DelEntity"))
      {
        m_WorldEditor.DestroyEntity(m_SelectedEntity.value());
        m_SelectedEntity.reset();
      }
    }

    const CUnorderedVector<ecs::TEntity> &Entities = m_WorldEditor.GetEntities();

    std::vector<std::string> EntitiesNames;
    for (auto Entity : Entities)
      EntitiesNames.push_back(std::to_string(Entity));

    int CurrentEntityIndex = GetSelectedEntityIndex(Entities);

    if (ImGui::ListBox(
            "Entities", &CurrentEntityIndex,
            [](void *_Vec, int _Index, const char **_Ret) -> bool {
              auto EntitiesNames = reinterpret_cast<std::vector<std::string> *>(_Vec);

              if (_Index < 0 || _Index >= EntitiesNames->size())
                return false;

              *_Ret = EntitiesNames->at(_Index).c_str();
              return true;
            },
            reinterpret_cast<void *>(&EntitiesNames), EntitiesNames.size(), 4))
    {
      m_SelectedEntity = Entities[CurrentEntityIndex];
    }

    if (m_SelectedEntity.has_value())
      RenderEntity(m_SelectedEntity.value());
  }

  if (ImGui::CollapsingHeader("Global parameters##GlobalParamsWindow"))
  {
    RenderGlobalParams();
  }
}

void CEditorUI::RenderEntity(ecs::TEntity _Entity)
{
  const CUnorderedVector<ecs::TComponentView> EntityComponents = m_WorldEditor.GetEntityComponents(_Entity);
  for (const ecs::TComponentView &ComponentView : EntityComponents)
  {
    if (ImGui::CollapsingHeader(ComponentView.Name.c_str()))
      m_ComponentRenderer.Render(ComponentView);
  }
}

int CEditorUI::GetSelectedEntityIndex(const CUnorderedVector<ecs::TEntity> &_Entities) const
{
  if (m_SelectedEntity.has_value())
  {
    const auto Iterator = _Entities.Find(m_SelectedEntity.value());
    if (Iterator == _Entities.end())
      return -1;

    return static_cast<int>(std::distance(_Entities.begin(), Iterator));
  }

  return -1;
}

void CEditorUI::SpawnEntity(ecs::TEntityType _Type)
{
  switch (_Type)
  {
  case ecs::TEntityType::StaticMesh: {
    const std::filesystem::path PathToLoad = utils::OpenFileDialog(utils::EFileDialogMode::SelectFile);
    if (PathToLoad.empty())
      return;

    std::shared_ptr<CModel> Model = resource::LoadModel(PathToLoad);
    if (!Model)
      return;

    auto &&TransformComponent = ecs::CComponentsFactory::Create<ecs::TTransformComponent>(glm::mat4x4(1.0f));
    auto &&ModelComponent     = ecs::CComponentsFactory::Create<ecs::TModelComponent>(Model);

    ecs::CEntitySpawner Builder = m_WorldEditor.CreateEntitySpawner();
    Builder.AddComponent(std::move(TransformComponent)).AddComponent(std::move(ModelComponent));
    m_SelectedEntity = Builder.Spawn();

    break;
  }

  case ecs::TEntityType::Skybox: {
    const std::filesystem::path PathToLoad = utils::OpenFileDialog(utils::EFileDialogMode::SelectFolder);
    if (PathToLoad.empty())
      return;

    std::shared_ptr<CTextureBase> Cubemap = resource::LoadCubemap(PathToLoad);
    if (!Cubemap)
      return;

    auto &&SkyboxComponent = ecs::CComponentsFactory::Create<ecs::TSkyboxComponent>(Cubemap);
    m_SelectedEntity       = m_WorldEditor.CreateEntitySpawner().AddComponent(std::move(SkyboxComponent)).Spawn();

    break;
  }

  case ecs::TEntityType::PointLight: {
    auto &&LightComponent = ecs::CComponentsFactory::Create<ecs::TLightComponent>(ELightType::Point);
    m_SelectedEntity      = m_WorldEditor.CreateEntitySpawner().AddComponent(std::move(LightComponent)).Spawn();

    break;
  }

  case ecs::TEntityType::DirectionalLight: {
    auto &&LightComponent = ecs::CComponentsFactory::Create<ecs::TLightComponent>(ELightType::Directional);
    m_SelectedEntity      = m_WorldEditor.CreateEntitySpawner().AddComponent(std::move(LightComponent)).Spawn();

    break;
  }

  case ecs::TEntityType::Spotlight: {
    auto &&LightComponent = ecs::CComponentsFactory::Create<ecs::TLightComponent>(ELightType::Spotlight);
    m_SelectedEntity      = m_WorldEditor.CreateEntitySpawner().AddComponent(std::move(LightComponent)).Spawn();

    break;
  }

  default:
    assert(false);
    break;
  }
}

} // namespace editor

#endif