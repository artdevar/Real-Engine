#pragma once

#include <filesystem>
#include "Shutdownable.h"

class IAsset : public IShutdownable
{
public:

  virtual ~IAsset() = default;

  virtual bool Load(const std::filesystem::path & _Path) = 0;
};