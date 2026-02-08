#pragma once

#include <filesystem>
#include "Shutdownable.h"
#include "engine/Passkey.h"

class CResourceManager;

class IAsset : public IShutdownable
{
public:
  virtual ~IAsset() = default;

  virtual bool Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>) = 0;
};