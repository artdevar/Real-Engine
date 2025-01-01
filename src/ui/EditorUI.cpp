#include "EditorUI.h"
#include "Shared.h"
#include "engine/Engine.h"
#include "engine/World.h"
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

void CEditorUI::Shutdown()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void CEditorUI::Init(CEngine * _Game)
{
  m_Engine = _Game;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(m_Engine->m_Window, true);
  ImGui_ImplOpenGL3_Init("#version 460");
  ImGui::StyleColorsDark();
}

void CEditorUI::Update(float _TimeDelta)
{

}

void CEditorUI::Render(CRenderer & _Renderer)
{
  RenderBegin();

  ImGui::Begin("Debug window##MainWindow");

  if (ImGui::Button("Load config"))
    m_Engine->LoadConfig();
  ImGui::SameLine();
  if (ImGui::Button("Save config"))
    m_Engine->SaveConfig();

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

void CEditorUI::RenderEntities()
{
  CWorld * World = m_Engine->m_World;

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
        std::make_pair("Static mesh",       TEntityType::StaticMesh),
        std::make_pair("Point light",       TEntityType::PointLight),
        std::make_pair("Directional light", TEntityType::DirectionalLight),
        std::make_pair("Spotlight",         TEntityType::Spotlight),
        std::make_pair("Skybox",            TEntityType::Skybox)
      };
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

    if (m_CurrentEntity.has_value())
    {
      ImGui::SameLine();
      if (ImGui::Button("Delete##DelEntity"))
      {
        World->RemoveEntity(m_CurrentEntity.value());
        m_CurrentEntity.reset();
      }
    }

    const std::vector<ecs::TEntity> & Entities = World->GetAllEntities();

    std::vector<std::string> EntitiesNames;
    for (auto Entity : Entities)
      EntitiesNames.push_back(std::to_string(Entity));

    int CurrentEntityIndex = GetCurrentEntityIndex(Entities);

    if (ImGui::ListBox(
          "Entities",
          &CurrentEntityIndex,
          [](void * _Vec, int _Index, const char ** _Ret) -> bool
          {
            auto EntitiesNames = reinterpret_cast<std::vector<std::string>*>(_Vec);

            if (_Index < 0 || _Index >= EntitiesNames->size())
              return false;

            *_Ret = EntitiesNames->at(_Index).c_str();
            return true;
          },
          reinterpret_cast<void*>(&EntitiesNames),
          EntitiesNames.size(),
          4)
        )
    {
      m_CurrentEntity = Entities[CurrentEntityIndex];
    }

    if (m_CurrentEntity.has_value())
      RenderEntityData(m_CurrentEntity.value());
  }
}

void CEditorUI::RenderEntityData(ecs::TEntity _Entity)
{
  CWorld * World = m_Engine->m_World;

  if (auto * Component = World->m_EntitiesCoordinator->GetComponentSafe<ecs::TModelComponent>(_Entity); Component)
    RenderEntityData(*Component);

  if (auto * Component = World->m_EntitiesCoordinator->GetComponentSafe<ecs::TTransformComponent>(_Entity); Component)
    RenderEntityData(*Component);

  if (auto * Component = World->m_EntitiesCoordinator->GetComponentSafe<ecs::TLightComponent>(_Entity); Component)
    RenderEntityData(*Component);
}

void CEditorUI::RenderEntityData(ecs::TModelComponent & _Mesh)
{
  //std::string ModelPath = _Mesh.Model->m_Path.string();
  //std::string ModelPath;
  //ImGui::InputText("Path##MeshPathInput", &ModelPath, ImGuiInputTextFlags_ReadOnly);
  //ImGui::SameLine();

  if (ImGui::Button("Select file"))
  {
    if (auto Filename = utils::OpenFileDialog(); !Filename.empty())
    {
      auto Model = m_Engine->GetResourceManager()->LoadModel(Filename);
      //if (Model)
      //  _Mesh.Model = std::move(Model);
    }
  }
}

void CEditorUI::RenderEntityData(ecs::TTransformComponent & _TransformComponent)
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

  ValueChanged |= ImGui::DragFloat("X##ObjXPos", &Translation.x);
  ValueChanged |= ImGui::DragFloat("Y##ObjYPos", &Translation.y);
  ValueChanged |= ImGui::DragFloat("Z##ObjZPos", &Translation.z);

  ImGui::SeparatorText("Scale##ObjScale");

  ValueChanged |= ImGui::DragFloat("X##ObjXScale", &Scale.x, 0.1f, 0.01f, std::numeric_limits<float>::max());
  ValueChanged |= ImGui::DragFloat("Y##ObjYScale", &Scale.y, 0.1f, 0.01f, std::numeric_limits<float>::max());
  ValueChanged |= ImGui::DragFloat("Z##ObjZScale", &Scale.z, 0.1f, 0.01f, std::numeric_limits<float>::max());

  ImGui::SeparatorText("Rotation##ObjRot");

  EulerAngles RotationAngles = utils::ToEulerAngles(Rotation);

  bool AngleChanged = false;
  AngleChanged |= ImGui::DragInt("Roll##EntRoll",   &RotationAngles.Roll);
  AngleChanged |= ImGui::DragInt("Pitch##EntPitch", &RotationAngles.Pitch);
  AngleChanged |= ImGui::DragInt("Yaw##EntYaw",     &RotationAngles.Yaw);
  ValueChanged |= AngleChanged;

  if (AngleChanged)
    Rotation = glm::quat({glm::radians(float(RotationAngles.Roll)), glm::radians(float(RotationAngles.Pitch)), glm::radians(float(RotationAngles.Yaw))});

  if (ValueChanged)
    _TransformComponent.Transform = glm::recompose(Scale, Rotation, Translation, Skew, Perspective);

  ImGui::Separator();
}

void CEditorUI::RenderEntityData(ecs::TLightComponent & _Light)
{
  int LightTypeVal = static_cast<int>(_Light.Type);

  ImGui::RadioButton("Directional", &LightTypeVal, static_cast<int>(ELightType::Directional)); ImGui::SameLine();
  ImGui::RadioButton("Point",       &LightTypeVal, static_cast<int>(ELightType::Point));       ImGui::SameLine();
  ImGui::RadioButton("Spotlight",   &LightTypeVal, static_cast<int>(ELightType::Spotlight));   ImGui::Separator();

  switch ((_Light.Type = static_cast<ELightType>(LightTypeVal)))
  {
    case ELightType::Directional:
    {
      ImGui::DragFloat3("Direction##LightDir", glm::value_ptr(_Light.Direction), 0.5f);
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
      ImGui::DragFloat3("Position##LightPos", glm::value_ptr(_Light.Position));
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
      ImGui::DragFloat3("Position##LightPos", glm::value_ptr(_Light.Position));
      ImGui::Separator();

      ImGui::DragFloat3("Direction##LightDir", glm::value_ptr(_Light.Direction));
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

int CEditorUI::GetCurrentEntityIndex(const std::vector<ecs::TEntity> & _Entities) const
{
  if (m_CurrentEntity.has_value())
  {
    const auto Iterator = std::find(_Entities.cbegin(), _Entities.cend(), m_CurrentEntity.value());
    return std::distance(_Entities.cbegin(), Iterator);
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

      CEntityBuilder Builder = m_Engine->m_World->GetEntityBuilder();

      Builder.AddComponent(ecs::CComponentsFactory::Create<ecs::TTransformComponent>(glm::mat4x4(1.0f)));
      Builder.AddComponent(ecs::CComponentsFactory::Create<ecs::TModelComponent>(Model));

      m_CurrentEntity = Builder.GetEntity();
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

      CEntityBuilder Builder = m_Engine->m_World->GetEntityBuilder();
      Builder.AddComponent(ecs::CComponentsFactory::Create<ecs::TSkyboxComponent>(Cubemap));
      m_CurrentEntity = Builder.GetEntity();

      break;
    }

    case TEntityType::PointLight:
    {
      CEntityBuilder Builder = m_Engine->m_World->GetEntityBuilder();
      Builder.AddComponent(ecs::CComponentsFactory::Create<ecs::TLightComponent>(ELightType::Point));
      m_CurrentEntity = Builder.GetEntity();
      break;
    }


    case TEntityType::DirectionalLight:
    {
      CEntityBuilder Builder = m_Engine->m_World->GetEntityBuilder();
      Builder.AddComponent(ecs::CComponentsFactory::Create<ecs::TLightComponent>(ELightType::Directional));
      m_CurrentEntity = Builder.GetEntity();
      break;
    }

    case TEntityType::Spotlight:
    {
      CEntityBuilder Builder = m_Engine->m_World->GetEntityBuilder();
      Builder.AddComponent(ecs::CComponentsFactory::Create<ecs::TLightComponent>(ELightType::Spotlight));
      m_CurrentEntity = Builder.GetEntity();
      break;
    }

    default:
      assert(false);
      break;
  }
}

