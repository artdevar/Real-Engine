#if DEV_STAGE

#include "pch.h"
#include "TransformComponentRenderer.h"
#include "ecs/Components.h"
#include <imgui/imgui.h>

namespace editor
{

ecs::TTypeID TTransformComponentRenderer::GetComponentTypeID() noexcept
{
  return ecs::utils::GetComponentTypeID<ecs::TTransformComponent>();
}

void TTransformComponentRenderer::Render(void *Data) noexcept
{
  auto *TransformComponent = static_cast<ecs::TTransformComponent *>(Data);

  glm::vec3 Scale;
  glm::quat Rotation;
  glm::vec3 Translation;
  glm::vec3 Skew;
  glm::vec4 Perspective;

  const bool Decomposed = glm::decompose(TransformComponent->WorldMatrix, Scale, Rotation, Translation, Skew, Perspective);
  assert(Decomposed);

  bool ValueChanged = false;

  ImGui::SeparatorText("Position##ObjPos");
  ValueChanged |= ImGui::DragFloat("X##ObjXPos", &Translation.x, 0.5f);
  ValueChanged |= ImGui::DragFloat("Y##ObjYPos", &Translation.y, 0.5f);
  ValueChanged |= ImGui::DragFloat("Z##ObjZPos", &Translation.z, 0.5f);

  ImGui::SeparatorText("Scale##ObjScale");
  const float kMinScale = 0.01f;
  const float kMaxScale = 10000.0f;
  ValueChanged         |= ImGui::DragFloat("X##ObjXScale", &Scale.x, 0.1f, kMinScale, kMaxScale);
  ValueChanged         |= ImGui::DragFloat("Y##ObjYScale", &Scale.y, 0.1f, kMinScale, kMaxScale);
  ValueChanged         |= ImGui::DragFloat("Z##ObjZScale", &Scale.z, 0.1f, kMinScale, kMaxScale);

  ImGui::SeparatorText("Rotation##ObjRotQuat");
  float quat[4]     = {Rotation.x, Rotation.y, Rotation.z, Rotation.w};
  bool  QuatChanged = false;
  QuatChanged      |= ImGui::DragFloat("X##ObjQuatX", &quat[0], 0.01f, -1.0f, 1.0f);
  QuatChanged      |= ImGui::DragFloat("Y##ObjQuatY", &quat[1], 0.01f, -1.0f, 1.0f);
  QuatChanged      |= ImGui::DragFloat("Z##ObjQuatZ", &quat[2], 0.01f, -1.0f, 1.0f);
  QuatChanged      |= ImGui::DragFloat("W##ObjQuatW", &quat[3], 0.01f, -1.0f, 1.0f);

  if (QuatChanged)
  {
    Rotation     = glm::quat(quat[3], quat[0], quat[1], quat[2]);
    Rotation     = glm::normalize(Rotation);
    ValueChanged = true;
  }

  if (ValueChanged)
  {
    Scale                           = glm::max(Scale, glm::vec3(kMinScale));
    TransformComponent->WorldMatrix = glm::recompose(Scale, Rotation, Translation, Skew, Perspective);
  }
}

} // namespace editor

#endif