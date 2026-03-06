#pragma once

#if DEV_STAGE

#include <ecs/Utils.h>
#include "RendererConcept.h"

namespace editor
{

struct TNameComponentRenderer
{
  static ecs::TTypeID GetComponentTypeID() noexcept;
  static void Render(void *Data) noexcept;
};

static_assert(ComponentRenderer<TNameComponentRenderer>, "TNameComponentRenderer must satisfy ComponentRenderer");

} // namespace editor

#endif
