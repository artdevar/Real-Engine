#pragma once

#include <filesystem>
#include <common/Core.h>

class CImage final
{
  DISABLE_CLASS_COPY(CImage);

public:
  explicit CImage(const std::filesystem::path &_Path);
  explicit CImage(const std::filesystem::path &_Path, bool _Flipped, bool _HDR);

  CImage(CImage &&_Other);
  CImage &operator=(CImage &&_Other);

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

  static bool IsHDR(const std::filesystem::path &_Path);

private:
  void LoadImage(const std::filesystem::path &_Path, bool _Flipped, bool _HDR);
  void FreeImage();

  void Clear();

private:
  void *m_Data;

  int m_Width;
  int m_Height;
  int m_Channels;
};
