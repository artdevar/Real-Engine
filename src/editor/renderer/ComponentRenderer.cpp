#if DEV_STAGE

#include "ComponentRenderer.h"
#include "ModelComponentRenderer.h"
#include "TransformComponentRenderer.h"
#include "LightComponentRenderer.h"
#include "SkyboxComponentRenderer.h"
#include "ecs/Components.h"
#include <ecs/ComponentView.h>
#include <ecs/Utils.h>
#include <imgui/imgui.h>

namespace editor
{

template <ComponentRenderer T>
void CComponentRenderer::RegisterComponentRenderer()
{
  m_RenderFunctions[T::GetComponentTypeID()] = T::Render;
}

CComponentRenderer::CComponentRenderer()
{
  RegisterComponentRenderer<TModelComponentRenderer>();
  RegisterComponentRenderer<TTransformComponentRenderer>();
  RegisterComponentRenderer<TLightComponentRenderer>();
  RegisterComponentRenderer<TSkyboxComponentRenderer>();
}

void CComponentRenderer::Render(ecs::TComponentView _ComponentView)
{
  auto Iter = m_RenderFunctions.find(_ComponentView.TypeID);
  if (Iter != m_RenderFunctions.end())
    Iter->second(_ComponentView.Data);
}

} // namespace editor

#endif