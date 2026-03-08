#pragma once

#if DEV_STAGE

#include <ecs/Utils.h>
#include "RendererConcept.h"

namespace editor
{

struct TCollisionComponentRenderer
{
  static ecs::TTypeID GetComponentTypeID() noexcept;
  static void Render(void *Data) noexcept;
};

static_assert(ComponentRenderer<TCollisionComponentRenderer>, "TCollisionComponentRenderer must satisfy ComponentRenderer");

} // namespace editor

#endif