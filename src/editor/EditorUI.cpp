#include "EditorUI.h"

#if EDITOR_ENABLED
#include "engine/Config.h"
#include "engine/Engine.h"
#include "engine/Display.h"
#include "scenes/World.h"
#include "engine/Camera.h"
#include "engine/ResourceManager.h"
#include "graphics/Model.h"
#include "graphics/Shader.h"
#include "ecs/Coordinator.h"
#include "ecs/EntityBuilder.h"
#include "ecs/ComponentsFactory.h"
#include "utils/Math.h"
#include "platform/SysUtils.h"
#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace
{
  bool ProjectWorldToScreen(const glm::vec3 &_WorldPos, const glm::mat4 &_View, const glm::mat4 &_Proj, const glm::ivec2 &_Viewport, ImVec2 &_Out)
  {
    const glm::vec4 Clip = _Proj * _View * glm::vec4(_WorldPos, 1.0f);
    if (Clip.w <= 0.0001f)
      return false;

    const glm::vec3 Ndc = glm::vec3(Clip) / Clip.w;
    if (Ndc.x < -1.0f || Ndc.x > 1.0f || Ndc.y < -1.0f || Ndc.y > 1.0f)
      return false;

    const float X = (Ndc.x * 0.5f + 0.5f) * _Viewport.x;
    const float Y = (1.0f - (Ndc.y * 0.5f + 0.5f)) * _Viewport.y;
    _Out = ImVec2(X, Y);
    return true;
  }
}

void CEditorUI::Shutdown()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void CEditorUI::Init(CEngine *_Engine)
{
  m_Engine = _Engine;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(m_Engine->GetDisplay()->GetWindow(), true);
  ImGui_ImplOpenGL3_Init("#version 460");
  ImGui::StyleColorsDark();
}

void CEditorUI::UpdateInternal(float _TimeDelta)
{
}

bool CEditorUI::ShouldBeUpdated() const
{
  return m_Engine != nullptr;
}

bool CEditorUI::ShouldBeRendered() const
{
  return m_Engine != nullptr;
}

void CEditorUI::RenderInternal(CRenderer &_Renderer)
{
  RenderBegin();

  ImGui::Begin("Main window##MainWindow");

  if (ImGui::Button("Load config"))
    m_Engine->LoadConfig();
  ImGui::SameLine();
  if (ImGui::Button("Save config"))
    m_Engine->SaveConfig();

  RenderEntities();
  RenderLightDebugLines();

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
  float CameraZNear = CConfig::Instance().GetCameraZNear();
  if (ImGui::DragFloat("Camera ZNear", &CameraZNear, 0.5f, -50.0f, 100.0f))
    CConfig::Instance().SetCameraZNear(CameraZNear, CPasskey(this));

  float CameraZFar = CConfig::Instance().GetCameraZFar();
  if (ImGui::DragFloat("Camera ZFar", &CameraZFar, 10.0f, 1.0f, 10000.0f))
    CConfig::Instance().SetCameraZFar(CameraZFar, CPasskey(this));

  float CameraFOV = CConfig::Instance().GetCameraFOV();
  if (ImGui::DragFloat("Camera FOV", &CameraFOV, 1.0f, 0.0f, 180.0f))
    CConfig::Instance().SetCameraFOV(CameraFOV, CPasskey(this));

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

void CEditorUI::RenderEntities()
{
  CWorld &World = *m_Engine->GetWorld();

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
      constexpr std::pair<const char *, TEntityType> EntityTypes[] = {
          std::make_pair("Static mesh", TEntityType::StaticMesh),
          // std::make_pair("Point light", TEntityType::PointLight),
          std::make_pair("Directional light", TEntityType::DirectionalLight),
          // std::make_pair("Spotlight", TEntityType::Spotlight),
          std::make_pair("Skybox", TEntityType::Skybox)};
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
        World.RemoveEntity(m_SelectedEntity.value());
        m_SelectedEntity.reset();
      }
    }

    const CUnorderedVector<ecs::TEntity> &Entities = World.GetAllEntities();

    std::vector<std::string> EntitiesNames;
    for (auto Entity : Entities)
      EntitiesNames.push_back(std::to_string(Entity));

    int CurrentEntityIndex = GetSelectedEntityIndex(Entities);

    if (ImGui::ListBox(
            "Entities",
            &CurrentEntityIndex,
            [](void *_Vec, int _Index, const char **_Ret) -> bool
            {
              auto EntitiesNames = reinterpret_cast<std::vector<std::string> *>(_Vec);

              if (_Index < 0 || _Index >= EntitiesNames->size())
                return false;

              *_Ret = EntitiesNames->at(_Index).c_str();
              return true;
            },
            reinterpret_cast<void *>(&EntitiesNames),
            EntitiesNames.size(),
            4))
    {
      m_SelectedEntity = Entities[CurrentEntityIndex];
    }

    if (m_SelectedEntity.has_value())
      RenderEntityData(m_SelectedEntity.value());
  }

  if (ImGui::CollapsingHeader("Global parameters##GlobalParamsWindow"))
  {
    RenderGlobalParams();
  }
}

void CEditorUI::RenderLightDebugLines()
{
  if (!m_SelectedEntity.has_value())
    return;

  CWorld &World = *m_Engine->GetWorld();
  const auto Camera = m_Engine->GetCamera();
  if (!Camera)
    return;

  const glm::mat4 View = Camera->GetView();
  const glm::mat4 Proj = Camera->GetProjection();
  const glm::ivec2 Viewport = m_Engine->GetWindowSize();

  ImDrawList *DrawList = ImGui::GetForegroundDrawList();

  auto *Light = World.m_EntitiesCoordinator->GetComponentSafe<ecs::TLightComponent>(m_SelectedEntity.value());
  if (!Light)
    return;

  glm::vec3 Direction = Light->Direction;
  if (glm::length(Direction) < 0.0001f)
    Direction = glm::vec3(0.0f, 1.0f, 0.0f);
  else
    Direction = glm::normalize(Direction);

  glm::vec3 StartWorld = Light->Position;
  if (auto *Transform = World.m_EntitiesCoordinator->GetComponentSafe<ecs::TTransformComponent>(m_SelectedEntity.value()); Transform)
    StartWorld = glm::vec3(Transform->Transform[3]);
  const glm::vec3 EndWorld = StartWorld + Direction * 1.0f;

  ImVec2 StartScreen;
  ImVec2 EndScreen;
  if (!ProjectWorldToScreen(StartWorld, View, Proj, Viewport, StartScreen) ||
      !ProjectWorldToScreen(EndWorld, View, Proj, Viewport, EndScreen))
    return;

  ImU32 Color = IM_COL32(255, 255, 0, 200);
  if (Light->Type == ELightType::Directional)
    Color = IM_COL32(255, 180, 0, 200);
  else if (Light->Type == ELightType::Spotlight)
    Color = IM_COL32(0, 200, 255, 200);

  DrawList->AddLine(StartScreen, EndScreen, Color, 2.0f);
  DrawList->AddCircleFilled(StartScreen, 3.0f, Color);
}

void CEditorUI::RenderEntityData(ecs::TEntity _Entity)
{
  CWorld &World = *m_Engine->GetWorld();

  if (auto *Component = World.m_EntitiesCoordinator->GetComponentSafe<ecs::TModelComponent>(_Entity); Component)
    RenderEntityData(*Component);

  if (auto *Component = World.m_EntitiesCoordinator->GetComponentSafe<ecs::TTransformComponent>(_Entity); Component)
    RenderEntityData(*Component);

  if (auto *Component = World.m_EntitiesCoordinator->GetComponentSafe<ecs::TLightComponent>(_Entity); Component)
    RenderEntityData(*Component);
}

void CEditorUI::RenderEntityData(ecs::TModelComponent &_Mesh)
{
  if (ImGui::Button("Select file"))
  {
    SpawnEntity(TEntityType::StaticMesh);
  }

  if (ImGui::Button("Dublicate"))
  {
    SpawnEntity(TEntityType::StaticMesh);
  }
}

void CEditorUI::RenderEntityData(ecs::TTransformComponent &_TransformComponent)
{
  glm::vec3 Scale;
  glm::quat Rotation;
  glm::vec3 Translation;
  glm::vec3 Skew;
  glm::vec4 Perspective;

  const bool Decomposed = glm::decompose(_TransformComponent.Transform, Scale, Rotation, Translation, Skew, Perspective);
  assert(Decomposed);

  bool ValueChanged = false;

  ImGui::SeparatorText("Position##ObjPos");

  ValueChanged |= ImGui::DragFloat("X##ObjXPos", &Translation.x, 0.5f);
  ValueChanged |= ImGui::DragFloat("Y##ObjYPos", &Translation.y, 0.5f);
  ValueChanged |= ImGui::DragFloat("Z##ObjZPos", &Translation.z, 0.5f);

  ImGui::SeparatorText("Scale##ObjScale");

  const float kMinScale = 0.01f;
  const float kMaxScale = 10000.0f; // reasonable upper bound to avoid extreme values
  ValueChanged |= ImGui::DragFloat("X##ObjXScale", &Scale.x, 0.1f, kMinScale, kMaxScale);
  ValueChanged |= ImGui::DragFloat("Y##ObjYScale", &Scale.y, 0.1f, kMinScale, kMaxScale);
  ValueChanged |= ImGui::DragFloat("Z##ObjZScale", &Scale.z, 0.1f, kMinScale, kMaxScale);

  ImGui::SeparatorText("Rotation##ObjRot");

  EulerAngles RotationAngles = utils::ToEulerAngles(Rotation);

  bool AngleChanged = false;
  AngleChanged |= ImGui::DragFloat("Roll##EntRoll", &RotationAngles.Roll, 1.0f);
  AngleChanged |= ImGui::DragFloat("Pitch##EntPitch", &RotationAngles.Pitch, 1.0f);
  AngleChanged |= ImGui::DragFloat("Yaw##EntYaw", &RotationAngles.Yaw, 1.0f);
  ValueChanged |= AngleChanged;

  if (AngleChanged)
  {
    const float kPitchLimit = 89.9f;
    RotationAngles.Pitch = glm::clamp(RotationAngles.Pitch, -kPitchLimit, kPitchLimit);

    Rotation = glm::quat(glm::radians(glm::vec3(RotationAngles.Roll, RotationAngles.Pitch, RotationAngles.Yaw)));
  }

  if (ValueChanged)
  {
    Scale = glm::max(Scale, glm::vec3(kMinScale));
    _TransformComponent.Transform = glm::recompose(Scale, Rotation, Translation, Skew, Perspective);
  }

  ImGui::Separator();
}

void CEditorUI::RenderEntityData(ecs::TLightComponent &_Light)
{
  int LightTypeVal = static_cast<int>(_Light.Type);

  // ImGui::RadioButton("Directional", &LightTypeVal, static_cast<int>(ELightType::Directional));
  // ImGui::SameLine();
  // ImGui::RadioButton("Point", &LightTypeVal, static_cast<int>(ELightType::Point));
  // ImGui::SameLine();
  // ImGui::RadioButton("Spotlight", &LightTypeVal, static_cast<int>(ELightType::Spotlight));
  // ImGui::Separator();

  switch ((_Light.Type = static_cast<ELightType>(LightTypeVal)))
  {
  case ELightType::Directional:
  {
    ImGui::DragFloat3("Direction##LightDir", glm::value_ptr(_Light.Direction), 0.25f);
    ImGui::Separator();

    ImGui::ColorEdit3("Ambient color##LightAmbColor", glm::value_ptr(_Light.Ambient), ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);
    ImGui::Separator();

    ImGui::ColorEdit3("Diffuse color##LightDiffColor", glm::value_ptr(_Light.Diffuse), ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);
    ImGui::Separator();

    ImGui::ColorEdit3("Specular color##LightSpecColor", glm::value_ptr(_Light.Specular), ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);
    break;
  }

  case ELightType::Point:
  {
    ImGui::DragFloat3("Position##LightPos", glm::value_ptr(_Light.Position), 0.25f);
    ImGui::Separator();

    ImGui::ColorEdit3("Ambient color##LightAmbColor", glm::value_ptr(_Light.Ambient), ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);
    ImGui::Separator();

    ImGui::ColorEdit3("Diffuse color##LightDiffColor", glm::value_ptr(_Light.Diffuse), ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);
    ImGui::Separator();

    ImGui::ColorEdit3("Specular color##LightSpecColor", glm::value_ptr(_Light.Specular), ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);
    ImGui::Separator();

    ImGui::DragFloat("LinearDrag", &_Light.Linear, 0.005f, 0.0f, 5.0f);
    ImGui::DragFloat("Quadratic", &_Light.Quadratic, 0.005f, 0.0f, 5.0f);
    break;
  }

  case ELightType::Spotlight:
  {
    ImGui::DragFloat3("Position##LightPos", glm::value_ptr(_Light.Position), 0.25f);
    ImGui::Separator();

    ImGui::DragFloat3("Direction##LightDir", glm::value_ptr(_Light.Direction), 0.25f);
    ImGui::Separator();

    ImGui::ColorEdit3("Ambient color##LightAmbColor", glm::value_ptr(_Light.Ambient), ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);
    ImGui::Separator();

    ImGui::ColorEdit3("Diffuse color##LightDiffColor", glm::value_ptr(_Light.Diffuse), ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);
    ImGui::Separator();

    ImGui::ColorEdit3("Specular color##LightSpecColor", glm::value_ptr(_Light.Specular), ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);
    break;
  }

  default:
    assert(false);
    break;
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

void CEditorUI::SpawnEntity(TEntityType _Type)
{
  switch (_Type)
  {
  case TEntityType::StaticMesh:
  {
    const std::filesystem::path ModelPathToLoad = utils::OpenFileDialog();
    if (ModelPathToLoad.empty())
      return;

    std::shared_ptr<CModel> Model = m_Engine->GetResourceManager()->LoadModel(ModelPathToLoad);
    if (!Model)
      return;

    CEntityBuilder Builder = m_Engine->GetWorld()->CreateEntity();

    Builder.AddComponent(ecs::CComponentsFactory::Create<ecs::TTransformComponent>(glm::mat4x4(1.0f)));
    Builder.AddComponent(ecs::CComponentsFactory::Create<ecs::TModelComponent>(Model));

    m_SelectedEntity = Builder.GetEntity();
    break;
  }

  case TEntityType::Skybox:
  {
    const std::filesystem::path ModelPathToLoad = utils::OpenFileDialog();
    if (ModelPathToLoad.empty())
      return;

    std::shared_ptr<CTextureBase> Cubemap = m_Engine->GetResourceManager()->LoadCubemap(ModelPathToLoad);
    if (!Cubemap)
      return;

    CEntityBuilder Builder = m_Engine->GetWorld()->CreateEntity();
    Builder.AddComponent(ecs::CComponentsFactory::Create<ecs::TSkyboxComponent>(Cubemap));
    m_SelectedEntity = Builder.GetEntity();

    break;
  }

  case TEntityType::PointLight:
  {
    CEntityBuilder Builder = m_Engine->GetWorld()->CreateEntity();
    Builder.AddComponent(ecs::CComponentsFactory::Create<ecs::TLightComponent>(ELightType::Point));
    m_SelectedEntity = Builder.GetEntity();
    break;
  }

  case TEntityType::DirectionalLight:
  {
    CEntityBuilder Builder = m_Engine->GetWorld()->CreateEntity();
    Builder.AddComponent(ecs::CComponentsFactory::Create<ecs::TLightComponent>(ELightType::Directional));
    m_SelectedEntity = Builder.GetEntity();
    break;
  }

  case TEntityType::Spotlight:
  {
    CEntityBuilder Builder = m_Engine->GetWorld()->CreateEntity();
    Builder.AddComponent(ecs::CComponentsFactory::Create<ecs::TLightComponent>(ELightType::Spotlight));
    m_SelectedEntity = Builder.GetEntity();
    break;
  }

  default:
    assert(false);
    break;
  }
}

#endif