#pragma once

#if DEV_STAGE

#include "RendererConcept.h"
#include <ecs/Core.h>
#include <unordered_map>
#include <functional>

namespace ecs
{
struct TComponentView;
}

namespace editor
{

class CComponentRenderer
{
public:
  CComponentRenderer();

  void Render(ecs::TComponentView _ComponentView);

private:
  template <ComponentRenderer T>
  void RegisterComponentRenderer();

private:
  std::unordered_map<ecs::TTypeID, RenderFunction> m_RenderFunctions;
};

} // namespace editor

#endif