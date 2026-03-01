#pragma once

#if DEV_STAGE

#include <ecs/Core.h>
#include <concepts>

namespace editor
{

template <typename T>
concept ComponentRenderer = requires {
  { T::GetComponentTypeID() } noexcept -> std::same_as<ecs::TTypeID>;
} && requires(void *data) {
  { T::Render(data) } noexcept -> std::same_as<void>;
};

using RenderFunction = void (*)(void *);

} // namespace editor

#endif