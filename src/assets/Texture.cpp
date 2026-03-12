#include "Texture.h"
#include "utils/Path.h"
#include <common/Logger.h>
#include <stb_image.h>
#include <vector>

namespace
{
constexpr GLint ToGLWrap(ETextureWrap _Wrap)
{
  switch (_Wrap)
  {
  case ETextureWrap::ClampToEdge:
    return GL_CLAMP_TO_EDGE;
  case ETextureWrap::ClampToBorder:
    return GL_CLAMP_TO_BORDER;
  case ETextureWrap::MirroredRepeat:
    return GL_MIRRORED_REPEAT;
  case ETextureWrap::Repeat:
    [[fallthrough]];
  default:
    return GL_REPEAT;
  }
}

constexpr GLint ToGLFilter(ETextureFilter _Filter)
{
  switch (_Filter)
  {
  case ETextureFilter::Nearest:
    return GL_NEAREST;
  case ETextureFilter::Linear:
    return GL_LINEAR;
  case ETextureFilter::LinearMipmapNearest:
    return GL_LINEAR_MIPMAP_NEAREST;
  case ETextureFilter::NearestMipmapNearest:
    return GL_NEAREST_MIPMAP_NEAREST;
  case ETextureFilter::NearestMipmapLinear:
    return GL_NEAREST_MIPMAP_LINEAR;
  case ETextureFilter::LinearMipmapLinear:
    [[fallthrough]];
  default:
    return GL_LINEAR_MIPMAP_LINEAR;
  }
}

constexpr GLint ToInternalFormat(int _Channels, bool _sRGB, bool _HDR)
{
  if (_HDR)
    return GL_RGB16F;

  switch (_Channels)
  {
  case 1:
    return GL_R8;
  case 2:
    return GL_RG8;
  case 3:
    return _sRGB ? GL_SRGB8 : GL_RGB8;
  case 4:
    return _sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
  default:
    assert(false && "Unsupported number of channels");
    return GL_RGB;
  }
}

constexpr GLint ToFormat(int _Channels)
{
  switch (_Channels)
  {
  case 1:
    return GL_RED;
  case 2:
    return GL_RG;
  case 3:
    return GL_RGB;
  case 4:
    return GL_RGBA;
  default:
    assert(false && "Unsupported number of channels");
    return GL_RGB;
  }
}
} // namespace

// CTextureBase

CTextureBase::~CTextureBase()
{
  assert(!IsValid() && "The texture isn't completely shutted down");
}

void CTextureBase::Shutdown()
{
  if (IsValid())
  {
    glDeleteTextures(1, &m_ID);
    m_ID = INVALID_VALUE;
  }
}

void CTextureBase::Bind(GLenum _TextureUnit)
{
  Bind(m_Target, _TextureUnit, ID());
}

void CTextureBase::Bind(GLenum _Target, GLenum _TextureUnit, GLuint _TextureID)
{
  glActiveTexture(_TextureUnit);
  glBindTexture(_Target, _TextureID);
}

void CTextureBase::Unbind()
{
  Unbind(m_Target);
}

void CTextureBase::Unbind(GLenum _Target)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(_Target, INVALID_VALUE);
}

GLuint CTextureBase::ID() const
{
  return m_ID;
}

bool CTextureBase::IsValid() const
{
  return m_ID != INVALID_VALUE;
}

TVector2i CTextureBase::GetSize() const
{
  return m_Size;
}

CTextureBase::CTextureBase(GLenum _Target) :
    m_ID(INVALID_VALUE),
    m_Target(_Target)
{
}

// CTexture

CTexture::CTexture() :
    CTextureBase(TARGET)
{
}

bool CTexture::Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>)
{
  TTextureParams Params;
  return Load(_Path, Params);
}

bool CTexture::Load(const std::filesystem::path &_Path, const TTextureParams &_Params, CPasskey<CResourceManager>)
{
  return Load(_Path, _Params);
}

bool CTexture::Load(const std::filesystem::path &_Path, const TTextureParams &_Params)
{
  assert(!IsValid() && "The texture already used");

  TImage Image;
  if (_Params.HDR)
  {
    stbi_set_flip_vertically_on_load(true);
    Image.Data = stbi_loadf(_Path.string().c_str(), &Image.Width, &Image.Height, &Image.Channels, 0);
    stbi_set_flip_vertically_on_load(false);
  }
  else
  {
    Image.Data = stbi_load(_Path.string().c_str(), &Image.Width, &Image.Height, &Image.Channels, 0);
  }

  if (!Image.Data)
  {
    CLogger::Log(ELogType::Error, "[CTexture] Texture '{}' failed to load", _Path.string());
    return false;
  }

  const GLenum Format         = ToFormat(Image.Channels);
  const GLenum InternalFormat = ToInternalFormat(Image.Channels, _Params.sRGB, _Params.HDR);

  glGenTextures(1, &m_ID);
  glBindTexture(m_Target, m_ID);
  glTexImage2D(m_Target, 0, InternalFormat, Image.Width, Image.Height, 0, Format, _Params.HDR ? GL_FLOAT : GL_UNSIGNED_BYTE, Image.Data);

  glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, ToGLWrap(_Params.WrapS));
  glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, ToGLWrap(_Params.WrapT));
  glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, ToGLFilter(_Params.MinFilter));
  glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, ToGLFilter(_Params.MagFilter));

  if (GLAD_GL_ARB_texture_filter_anisotropic)
    glTexParameterf(m_Target, GL_TEXTURE_MAX_ANISOTROPY, _Params.Anisotropy);

  if (!_Params.HDR)
    glGenerateMipmap(m_Target);

  if (_Params.BorderColors.has_value())
    glTexParameterfv(m_Target, GL_TEXTURE_BORDER_COLOR, _Params.BorderColors->Data());

  stbi_image_free(Image.Data);

  m_Path = _Path;
  m_Size = TVector2i(Image.Width, Image.Height);

  return true;
}

bool CTexture::Generate(const TTextureParams &_Params, CPasskey<CResourceManager>)
{
  if (_Params.Width <= 0 || _Params.Height <= 0)
  {
    CLogger::Log(ELogType::Error, "[CTexture] Invalid texture size: {}x{}", _Params.Width, _Params.Height);
    return false;
  }

  assert(!IsValid() && "The texture already exists");

  glGenTextures(1, &m_ID);
  glBindTexture(m_Target, m_ID);
  glTexImage2D(m_Target, 0, _Params.InternalFormat, _Params.Width, _Params.Height, 0, _Params.Format, _Params.Type, NULL);

  glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, ToGLWrap(_Params.WrapS));
  glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, ToGLWrap(_Params.WrapT));
  glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, ToGLFilter(_Params.MinFilter));
  glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, ToGLFilter(_Params.MagFilter));

  if (GLAD_GL_ARB_texture_filter_anisotropic)
    glTexParameterf(m_Target, GL_TEXTURE_MAX_ANISOTROPY, _Params.Anisotropy);

  if (_Params.BorderColors.has_value())
    glTexParameterfv(m_Target, GL_TEXTURE_BORDER_COLOR, _Params.BorderColors->Data());

  m_Size = TVector2i(_Params.Width, _Params.Height);
  m_Path = "Generated Texture";

  return true;
}

void CTexture::Bind(GLenum _TextureUnit, GLuint _TextureID)
{
  CTextureBase::Bind(TARGET, _TextureUnit, _TextureID);
}

void CTexture::Unbind()
{
  CTextureBase::Unbind(TARGET);
}

// CCubemap

CCubemap::CCubemap() :
    CTextureBase(TARGET)
{
}

bool CCubemap::Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>)
{
  TTextureParams Params;
  Params.WrapS     = ETextureWrap::ClampToEdge;
  Params.WrapT     = ETextureWrap::ClampToEdge;
  Params.WrapR     = ETextureWrap::ClampToEdge;
  Params.MinFilter = ETextureFilter::Linear;
  Params.MagFilter = ETextureFilter::Linear;
  return Load(_Path, Params);
}

bool CCubemap::Load(const std::filesystem::path &_Path, const TTextureParams &Params, CPasskey<CResourceManager>)
{
  return Load(_Path, Params);
}

bool CCubemap::Generate(const TTextureParams &_Params, CPasskey<CResourceManager>)
{
  if (_Params.Width <= 0 || _Params.Height <= 0)
  {
    CLogger::Log(ELogType::Error, "[CTexture] Invalid texture size: {}x{}", _Params.Width, _Params.Height);
    return false;
  }

  assert(!IsValid() && "The texture already exists");

  glGenTextures(1, &m_ID);
  glBindTexture(m_Target, m_ID);

  for (int i = 0; i < CUBEMAP_FACES_COUNT; ++i)
  {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, //
                 0,                                  //
                 _Params.InternalFormat,             //
                 _Params.Width,                      //
                 _Params.Height,                     //
                 0,                                  //
                 _Params.Format,                     //
                 _Params.Type,                       //
                 nullptr);
  }

  glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, ToGLWrap(_Params.WrapS));
  glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, ToGLWrap(_Params.WrapT));
  glTexParameteri(m_Target, GL_TEXTURE_WRAP_R, ToGLWrap(_Params.WrapR));
  glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, ToGLFilter(_Params.MinFilter));
  glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, ToGLFilter(_Params.MagFilter));

  m_Size = TVector2i(_Params.Width, _Params.Height);
  m_Path = "Generated Cubemap";

  return true;
}

bool CCubemap::Load(const std::filesystem::path &_Path, const TTextureParams &_Params)
{
  return LoadLegacy(_Path, _Params);
}

bool CCubemap::LoadLegacy(const std::filesystem::path &_Path, const TTextureParams &_Params)
{
  static const CStaticArray<std::string, CUBEMAP_FACES_COUNT> CUBEMAP_FACES = {"right", "left", "top", "bottom", "front", "back"};

  const auto Files   = utils::GetFilesInDirectory(_Path);
  bool       Success = false;

  CStaticArray<TImage, CUBEMAP_FACES_COUNT> Images;
  for (const std::string &Face : CUBEMAP_FACES)
  {
    const auto It = std::find_if(Files.begin(), Files.end(), [&Face](const std::filesystem::path &File) {
      return File.stem().string() == Face;
    });

    if (It == Files.end())
    {
      CLogger::Log(ELogType::Error, "[CCubemap] Texture '{}' failed to load: missing face '{}'", _Path.string(), Face);
      break;
    }

    TImage Image;
    Image.Data = stbi_load(It->string().c_str(), &Image.Width, &Image.Height, &Image.Channels, 0);
    if (Image.Data)
      Images.PushBack(std::move(Image));
  }

  if (Success = (Images.GetActualSize() == CUBEMAP_FACES_COUNT))
  {
    glGenTextures(1, &m_ID);
    glBindTexture(m_Target, m_ID);

    for (int i = 0; i < Images.GetActualSize(); ++i)
    {
      const TImage &Image  = Images[i];
      const GLenum  Format = ToFormat(Image.Channels);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, Format, Image.Width, Image.Height, 0, Format, GL_UNSIGNED_BYTE, Image.Data);
    }

    glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, ToGLFilter(_Params.MinFilter));
    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, ToGLFilter(_Params.MagFilter));
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, ToGLWrap(_Params.WrapS));
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, ToGLWrap(_Params.WrapT));
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_R, ToGLWrap(_Params.WrapR));

    m_Size = TVector2i(Images[0].Width, Images[0].Height);

    CLogger::Log(ELogType::Info, "[CCubemap] Texture '{}' loaded successfully", _Path.string());
  }
  else
  {
    CLogger::Log(ELogType::Error, "[CCubemap] Texture '{}' failed to load", _Path.string());
  }

  for (TImage &Image : Images)
    stbi_image_free(Image.Data);

  m_Path = _Path;

  return Success;
}

void CCubemap::Bind(GLenum _TextureUnit, GLuint _TextureID)
{
  CTextureBase::Bind(TARGET, _TextureUnit, _TextureID);
}

void CCubemap::Unbind()
{
  CTextureBase::Unbind(TARGET);
}