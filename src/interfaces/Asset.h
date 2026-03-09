#pragma once

#include "engine/Passkey.h"
#include <common/interfaces/Shutdownable.h>
#include <filesystem>

class CResourceManager;

class IAsset : public IShutdownable
{
public:
  virtual ~IAsset() = default;

  virtual bool Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>) = 0;

#if DEV_STAGE
  std::filesystem::path GetPath() const
  {
    return m_Path;
  }

protected:
  std::filesystem::path m_Path;
#endif
};