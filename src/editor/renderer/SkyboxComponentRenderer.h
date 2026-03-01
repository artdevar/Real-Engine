#pragma once

#if DEV_STAGE

#include <ecs/Utils.h>
#include "RendererConcept.h"

namespace editor
{

struct TSkyboxComponentRenderer
{
  static ecs::TTypeID GetComponentTypeID() noexcept;
  static void Render(void *Data) noexcept;
};

static_assert(ComponentRenderer<TSkyboxComponentRenderer>, "TSkyboxComponentRenderer must satisfy ComponentRenderer");

} // namespace editor

#endif