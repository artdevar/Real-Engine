#pragma once

#include <common/Core.h>
#include <common/Passkey.h>
#include <common/interfaces/Shutdownable.h>
#include <filesystem>

class CResourceManager;

class IAsset : public IShutdownable
{
  DISABLE_CLASS_COPY(IAsset);

public:
  IAsset()          = default;
  virtual ~IAsset() = default;

  virtual bool Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>) = 0;

  std::filesystem::path GetPath() const
  {
    return m_Path;
  }

protected:
  std::filesystem::path m_Path;
};
