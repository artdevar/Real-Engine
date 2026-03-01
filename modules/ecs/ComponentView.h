#pragma once

#include "Core.h"

namespace ecs
{

struct TComponentView
{
  TTypeName Name;
  TTypeID   TypeID;
  void     *Data;
};

} // namespace ecs