#include <glad/glad.h>
#include "Texture.h"
#include "utils/Path.h"
#include "utils/Image.h"
#include <common/Logger.h>
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

constexpr GLint ToGLInternalFormat(int _Channels, bool _sRGB, bool _HDR)
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

constexpr GLint ToGLInternalFormat(EInternalFormat _Format)
{
  switch (_Format)
  {
  case EInternalFormat::R8:
    return GL_R8;
  case EInternalFormat::RG8:
    return GL_RG8;
  case EInternalFormat::RGB8:
    return GL_RGB8;
  case EInternalFormat::RGBA8:
    return GL_RGBA8;
  case EInternalFormat::RG16F:
    return GL_RG16F;
  case EInternalFormat::RGB16F:
    return GL_RGB16F;
  case EInternalFormat::RGBA16F:
    return GL_RGBA16F;
  case EInternalFormat::Depth16:
    return GL_DEPTH_COMPONENT16;
  case EInternalFormat::Depth24:
    return GL_DEPTH_COMPONENT24;
  case EInternalFormat::Depth32:
    return GL_DEPTH_COMPONENT32;
  default:
    assert(false && "Unsupported internal format");
    return GL_RGB8;
  }
}

constexpr GLint ToGLFormat(int _Channels)
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

constexpr GLint ToGLFormat(EFormat _Format)
{
  switch (_Format)
  {
  case EFormat::Red:
    return GL_RED;
  case EFormat::RG:
    return GL_RG;
  case EFormat::RGB:
    return GL_RGB;
  case EFormat::RGBA:
    return GL_RGBA;
  case EFormat::Depth:
    return GL_DEPTH_COMPONENT;
  default:
    assert(false && "Unsupported format");
    return GL_RGB;
  }
}

constexpr GLint ToGLType(EType _Type)
{
  switch (_Type)
  {
  case EType::Byte:
    return GL_BYTE;
  case EType::UnsignedByte:
    return GL_UNSIGNED_BYTE;
  case EType::Short:
    return GL_SHORT;
  case EType::UnsignedShort:
    return GL_UNSIGNED_SHORT;
  case EType::Int:
    return GL_INT;
  case EType::UnsignedInt:
    return GL_UNSIGNED_INT;
  case EType::Float:
    return GL_FLOAT;
  default:
    assert(false && "Unsupported type");
    return GL_UNSIGNED_BYTE;
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

const unsigned CTexture::TARGET = GL_TEXTURE_2D;

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

  const CImage Image(_Path, _Params.HDR, _Params.HDR);
  if (!Image.IsValid())
  {
    CLogger::Log(ELogType::Error, "[CTexture] Texture '{}' failed to load", _Path.string());
    return false;
  }

  const GLenum Format         = ToGLFormat(Image.GetChannels());
  const GLenum InternalFormat = ToGLInternalFormat(Image.GetChannels(), _Params.sRGB, _Params.HDR);

  glGenTextures(1, &m_ID);
  glBindTexture(m_Target, m_ID);
  glTexImage2D(m_Target, 0, InternalFormat, Image.GetWidth(), Image.GetHeight(), 0, Format, _Params.HDR ? GL_FLOAT : GL_UNSIGNED_BYTE,
               Image.GetPixels());

  glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, ToGLWrap(_Params.WrapS));
  glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, ToGLWrap(_Params.WrapT));
  glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, ToGLFilter(_Params.MinFilter));
  glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, ToGLFilter(_Params.MagFilter));

  if (GLAD_GL_ARB_texture_filter_anisotropic)
    glTexParameterf(m_Target, GL_TEXTURE_MAX_ANISOTROPY, std::min(GetSupportedAnisotropyLevel(), _Params.Anisotropy));

  if (!_Params.HDR)
    glGenerateMipmap(m_Target);

  if (_Params.BorderColors.has_value())
    glTexParameterfv(m_Target, GL_TEXTURE_BORDER_COLOR, _Params.BorderColors->Data());

  m_Path = _Path;
  m_Size = TVector2i(Image.GetWidth(), Image.GetHeight());

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

  const GLint InternalFormat = ToGLInternalFormat(_Params.InternalFormat);
  const GLint Format         = ToGLFormat(_Params.Format);
  const GLint Type           = ToGLType(_Params.Type);

  glGenTextures(1, &m_ID);
  glBindTexture(m_Target, m_ID);
  glTexImage2D(m_Target, 0, InternalFormat, _Params.Width, _Params.Height, 0, Format, Type, _Params.Data);

  glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, ToGLWrap(_Params.WrapS));
  glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, ToGLWrap(_Params.WrapT));
  glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, ToGLFilter(_Params.MinFilter));
  glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, ToGLFilter(_Params.MagFilter));

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

float CTexture::GetSupportedAnisotropyLevel()
{
  static const float AnisotropyLevel = []() -> float {
    float MaxAnisotropy = 0.0f;
    if (GLAD_GL_ARB_texture_filter_anisotropic)
    {
      glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &MaxAnisotropy);
      LOG_INFO("[CTexture] Max anisotropy level supported: {}", MaxAnisotropy);
    }
    else
    {
      LOG_INFO("[CTexture] Anisotropic filtering is not supported");
    }
    return MaxAnisotropy;
  }();

  return AnisotropyLevel;
}

// CCubemap

const unsigned CCubemap::TARGET = GL_TEXTURE_CUBE_MAP;

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

  const GLint InternalFormat = ToGLInternalFormat(_Params.InternalFormat);
  const GLint Format         = ToGLFormat(_Params.Format);
  const GLint Type           = ToGLType(_Params.Type);

  glGenTextures(1, &m_ID);
  glBindTexture(m_Target, m_ID);

  for (int i = 0; i < CUBEMAP_FACES_COUNT; ++i)
  {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, //
                 0,                                  //
                 InternalFormat,                     //
                 _Params.Width,                      //
                 _Params.Height,                     //
                 0,                                  //
                 Format,                             //
                 Type,                               //
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

  const auto Files = utils::GetFilesInDirectory(_Path);

  std::vector<CImage> Images;
  Images.reserve(CUBEMAP_FACES_COUNT);

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

    CImage Image(*It);
    if (Image.IsValid())
      Images.push_back(std::move(Image));
    else
      break;
  }

  bool Success = false;
  if (Success = (Images.size() == CUBEMAP_FACES_COUNT))
  {
    glGenTextures(1, &m_ID);
    glBindTexture(m_Target, m_ID);

    for (int i = 0; i < Images.size(); ++i)
    {
      const CImage &Image  = Images[i];
      const GLenum  Format = ToGLFormat(Image.GetChannels());
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, Format, Image.GetWidth(), Image.GetHeight(), 0, Format, GL_UNSIGNED_BYTE,
                   Image.GetPixels());
    }

    glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, ToGLFilter(_Params.MinFilter));
    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, ToGLFilter(_Params.MagFilter));
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, ToGLWrap(_Params.WrapS));
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, ToGLWrap(_Params.WrapT));
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_R, ToGLWrap(_Params.WrapR));

    m_Size = TVector2i(Images[0].GetWidth(), Images[0].GetHeight());
    m_Path = _Path;

    CLogger::Log(ELogType::Info, "[CCubemap] Texture '{}' loaded successfully", _Path.string());
  }
  else
  {
    CLogger::Log(ELogType::Error, "[CCubemap] Texture '{}' failed to load", _Path.string());
  }

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
