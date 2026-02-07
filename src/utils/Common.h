#pragma once

#include <filesystem>
#include <algorithm>
#include <execution>

#define DISABLE_CLASS_COPY(Class) \
  Class(const Class &) = delete;  \
  Class &operator=(const Class &) = delete;

#define ENABLE_CLASS_MOVE(Class) \
  Class(Class &&) = default;     \
  Class &operator=(Class &&) = default;

#define ARRAY_SIZE(Array) \
  sizeof((Array)) / sizeof((Array)[0])

#define IS_SAME_TYPE(Lhs, Rhs) \
  static_assert(std::is_same_v<std::decay_t<decltype((Lhs))>, std::decay_t<decltype((Rhs))>>)

template <typename Destination, typename Source>
  requires(std::is_trivially_copyable_v<Destination> && std::is_trivially_copyable_v<Source>)
inline void FastMemCpy(Destination *_Dst, Source *_Src, uint32_t _SizeInBytes)
{
  std::copy(std::execution::par, (uint8_t *)_Src, (uint8_t *)_Src + _SizeInBytes, (uint8_t *)_Dst);
}

template <typename Destination, typename T>
  requires(std::is_standard_layout_v<Destination> && std::is_trivial_v<T>)
inline void FastMemSet(Destination *_Dst, T &&_Value, uint32_t _SizeInBytes)
{
  std::fill(std::execution::par, (uint8_t *)_Dst, (uint8_t *)_Dst + _SizeInBytes, std::forward<T>(_Value));
}
