#pragma once

#include "Components.h"

namespace ecs
{

  class CComponentsFactory final
  {
  public:
    CComponentsFactory() = delete;

    template <typename Component>
      requires(std::is_same_v<Component, TModelComponent>)
    [[nodiscard]] static Component Create(const std::shared_ptr<CModel> &_Model)
    {
      assert(_Model != nullptr);

      TModelComponent ModelComponent;
      CreateModelComponent(_Model, ModelComponent);

      return ModelComponent;
    }

    template <typename Component>
      requires(std::is_same_v<Component, TSkyboxComponent>)
    [[nodiscard]] static Component Create(const std::shared_ptr<CTextureBase> &_Skybox)
    {
      TSkyboxComponent SkyboxComponent;
      CreateSkyboxComponent(_Skybox, SkyboxComponent);

      return SkyboxComponent;
    }

    template <typename Component>
      requires(std::is_same_v<Component, TLightComponent>)
    [[nodiscard]] static Component Create(ELightType _Type)
    {
      TLightComponent LightComponent;
      LightComponent.Type = _Type;
      LightComponent.Direction = glm::vec3(0.0f, -1.0f, 0.0001f);
      LightComponent.Ambient = glm::vec3(0.1f, 0.1f, 0.1f);
      LightComponent.Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
      LightComponent.Specular = glm::vec3(1.0f, 1.0f, 1.0f);

      return LightComponent;
    }

    template <typename Component>
      requires(std::is_same_v<Component, TTransformComponent>)
    [[nodiscard]] static Component Create(glm::mat4x4 _Transform)
    {
      TTransformComponent TransformComponent;
      TransformComponent.WorldMatrix = _Transform;

      return TransformComponent;
    }

  private:
    static void CreateModelComponent(const std::shared_ptr<CModel> &_Model, TModelComponent &_Component);
    static void CreateSkyboxComponent(const std::shared_ptr<CTextureBase> &_Skybox, TSkyboxComponent &_Component);
  };

}
