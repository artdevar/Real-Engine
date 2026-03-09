#include "GizmoRenderer.h"
#include "ecs/Components.h"
#include "interfaces/WorldEditor.h"
#include "engine/Camera.h"
#include "engine/Engine.h"
#include <imgui/imgui.h>
#include <imgui/ImGuizmo/ImGuizmo.h>

namespace editor
{

CGizmoRenderer::CGizmoRenderer(IWorldEditor &_WorldEditor) :
    m_WorldEditor(_WorldEditor),
    m_Operations({ImGuizmo::TRANSLATE, ImGuizmo::ROTATE, ImGuizmo::SCALE}),
    m_CurrentOperation(0)
{
}

void CGizmoRenderer::Render(ecs::TEntity _Entity, TRectf _ViewportRect)
{
  if (ImGui::GetIO().MouseWheel != 0.0f)
    m_CurrentOperation = (m_CurrentOperation + 1) % m_Operations.GetActualSize();

  ecs::TTransformComponent *Transform = m_WorldEditor.GetTransform(_Entity);
  if (!Transform)
    return;

  ImGuizmo::SetDrawlist();
  ImGuizmo::SetRect(_ViewportRect.Position.X, _ViewportRect.Position.Y, _ViewportRect.Size.X, _ViewportRect.Size.Y);

  std::shared_ptr<CCamera> Camera    = CEngine::Instance().GetCamera();
  const glm::mat4          View      = Camera->GetView();
  const glm::mat4          Proj      = Camera->GetPerspectiveProjection();
  glm::mat4                Model     = Transform->WorldMatrix;
  const auto               Operation = static_cast<ImGuizmo::OPERATION>(m_Operations[m_CurrentOperation]);

  if (ImGuizmo::Manipulate(glm::value_ptr(View), glm::value_ptr(Proj), Operation, ImGuizmo::LOCAL, glm::value_ptr(Model)))
    Transform->WorldMatrix = Model;
}

} // namespace editor