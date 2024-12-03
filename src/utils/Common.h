#pragma once

#include <cstring>

#define DISABLE_CLASS_COPY(Class) \
  Class(const Class &) = delete; \
  Class & operator=(const Class &) = delete;

#define ENABLE_CLASS_MOVE(Class) \
  Class(Class &&) = default; \
  Class & operator=(Class &&) = default;

#define ARRAY_SIZE(Array) \
  sizeof((Array)) / sizeof((Array)[0])

#define MEM_ZERO(Data) \
  std::memset(&(Data), 0, sizeof((Data)))

