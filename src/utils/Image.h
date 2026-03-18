#pragma once

#include <filesystem>
#include <common/Core.h>

class CImage final
{
  DISABLE_CLASS_COPY(CImage);

public:
  explicit CImage(const std::filesystem::path &_Path);
  ~CImage();

  int GetWidth() const
  {
    return m_Width;
  }
  int GetHeight() const
  {
    return m_Height;
  }
  int GetChannels() const
  {
    return m_Channels;
  }
  void *GetPixels() const
  {
    return m_Data;
  }
  bool IsValid() const
  {
    return m_Data != nullptr;
  }

private:
  void LoadImage(const std::filesystem::path &_Path);
  void FreeImage();

private:
  void *m_Data;

  int m_Width;
  int m_Height;
  int m_Channels;
};
