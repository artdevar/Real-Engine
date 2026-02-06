#pragma once

#include <filesystem>
#include "Shutdownable.h"

class CResourceManagerKey
{
private:
  CResourceManagerKey() = default;
  friend class CResourceManager;
};

class IAsset : public IShutdownable
{
public:
  virtual ~IAsset() = default;

  virtual bool Load(const std::filesystem::path &_Path, CResourceManagerKey) = 0;
};