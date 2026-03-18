#include "Image.h"
#include <stb_image.h>

CImage::CImage(const std::filesystem::path &_Path) :
    m_Data(nullptr),
    m_Width(0),
    m_Height(0),
    m_Channels(0)
{
  LoadImage(_Path);
}

CImage::~CImage()
{
  FreeImage();
}

void CImage::LoadImage(const std::filesystem::path &_Path)
{
  m_Data = stbi_load(_Path.string().c_str(), &m_Width, &m_Height, &m_Channels, 0);
}

void CImage::FreeImage()
{
  if (IsValid())
    stbi_image_free(GetPixels());
}
