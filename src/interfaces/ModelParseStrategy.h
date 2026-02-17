#pragma once

#include "graphics/ModelData.h"
#include <filesystem>

class IModelParseStrategy
{
public:
  virtual ~IModelParseStrategy() = default;

  virtual bool Parse(const std::filesystem::path &_Path, TModelData &_Model /* Output */) = 0;
};