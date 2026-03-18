#include "Image.h"
#include <stb_image.h>
#include <cassert>

CImage::CImage(const std::filesystem::path &_Path) :
    m_Data(nullptr),
    m_Width(0),
    m_Height(0),
    m_Channels(0)
{
  LoadImage(_Path, false, false);
}

CImage::CImage(const std::filesystem::path &_Path, bool _Flipped, bool _HDR) :
    m_Data(nullptr),
    m_Width(0),
    m_Height(0),
    m_Channels(0)
{
  LoadImage(_Path, _Flipped, _HDR);
}

CImage::~CImage()
{
  FreeImage();
}

CImage::CImage(CImage &&_Other) :
    m_Data(_Other.m_Data),
    m_Width(_Other.m_Width),
    m_Height(_Other.m_Height),
    m_Channels(_Other.m_Channels)
{
  _Other.Clear();
}

CImage &CImage::operator=(CImage &&_Other)
{
  if (this != &_Other)
  {
    FreeImage();

    m_Data     = _Other.m_Data;
    m_Width    = _Other.m_Width;
    m_Height   = _Other.m_Height;
    m_Channels = _Other.m_Channels;

    _Other.Clear();
  }

  return *this;
}

void CImage::LoadImage(const std::filesystem::path &_Path, bool _Flipped, bool _HDR)
{
  const std::string PathStr = _Path.string();
  stbi_set_flip_vertically_on_load(_Flipped);

  if (_HDR)
  {
    assert(IsHDR(_Path));
    m_Data = stbi_loadf(PathStr.c_str(), &m_Width, &m_Height, &m_Channels, 0);
  }
  else
  {
    assert(!IsHDR(_Path));
    m_Data = stbi_load(PathStr.c_str(), &m_Width, &m_Height, &m_Channels, 0);
  }

  stbi_set_flip_vertically_on_load(false);
}

void CImage::FreeImage()
{
  if (IsValid())
  {
    stbi_image_free(GetPixels());
    Clear();
  }
}

void CImage::Clear()
{
  m_Data     = nullptr;
  m_Width    = 0;
  m_Height   = 0;
  m_Channels = 0;
}

bool CImage::IsHDR(const std::filesystem::path &_Path)
{
  const std::string PathStr = _Path.string();
  return stbi_is_hdr(PathStr.c_str()) != 0;
}
