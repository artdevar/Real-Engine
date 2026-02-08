#pragma once

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
