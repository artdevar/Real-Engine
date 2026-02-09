#pragma once

#include <glad/glad.h>
#include <nlohmann/json.hpp>
#include "interfaces/Asset.h"
#include "interfaces/GeneratableTexture.h"
#include "utils/Common.h"
#include "utils/Logger.h"
#include "utils/Json.h"
#include "stb_image.h"

class CTextureBase : public IAsset,
                     public IGeneratableTexture
{
  DISABLE_CLASS_COPY(CTextureBase);

public:
  ~CTextureBase() override
  {
    assert(!IsValid() && "The texture isn't completely shutted down");
  }

  void Shutdown() override
  {
    if (IsValid())
    {
      glDeleteTextures(1, &m_ID);
      m_ID = INVALID_VALUE;
    }
  }

  void Bind(GLenum _TextureUnit)
  {
    glActiveTexture(_TextureUnit);
    glBindTexture(m_Target, Get());
  }

  void Unbind()
  {
    // glActiveTexture(0); opengl error
    glBindTexture(m_Target, INVALID_VALUE);
  }

  GLuint Get() const
  {
    return m_ID;
  }

  bool IsValid() const
  {
    return m_ID != INVALID_VALUE;
  }

protected:
  struct TImage
  {
    unsigned char *Data;

    int Width;
    int Height;
    int Channels;
  };

  CTextureBase(GLenum _Target) : m_ID(INVALID_VALUE),
                                 m_Target(_Target)
  {
  }

  static constexpr inline GLuint INVALID_VALUE = 0u;

  GLuint m_ID;
  const GLenum m_Target;
};

//

class CTexture final : public CTextureBase
{
public:
  CTexture() : CTextureBase(GL_TEXTURE_2D) {}

  bool Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>) override
  {
    TImage Image;
    Image.Data = stbi_load(_Path.c_str(), &Image.Width, &Image.Height, &Image.Channels, 0);

    if (!Image.Data)
    {
      CLogger::Log(ELogType::Error, "[CTexture] Texture '{}' failed to load", _Path.c_str());
      return false;
    }

    constexpr GLenum Formats[] = {GL_RED, GL_RED, GL_RGB, GL_RGBA};
    const GLenum Format = Formats[Image.Channels - 1];

    glGenTextures(1, &m_ID);
    glBindTexture(m_Target, m_ID);
    glTexImage2D(m_Target, 0, Format, Image.Width, Image.Height, 0, Format, GL_UNSIGNED_BYTE, Image.Data);
    glGenerateMipmap(m_Target);

    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(Image.Data);

    CLogger::Log(ELogType::Debug, "[CTexture] Texture '{}' loaded successfully", _Path.c_str());

    return true;
  }

  bool Generate(const TTextureParams &_Params, CPasskey<CResourceManager>) override
  {
    if (_Params.Width <= 0 || _Params.Height <= 0)
    {
      CLogger::Log(ELogType::Error, "[CTexture] Invalid texture size: {}x{}", _Params.Width, _Params.Height);
      return false;
    }

    assert(!IsValid() && "The texture already exists");

    auto ToGLWrap = [](ETextureWrap _Wrap)
    {
      switch (_Wrap)
      {
      case ETextureWrap::ClampToEdge:
        return GL_CLAMP_TO_EDGE;
      case ETextureWrap::MirroredRepeat:
        return GL_MIRRORED_REPEAT;
      case ETextureWrap::Repeat:
      default:
        return GL_REPEAT;
      }
    };

    auto ToGLFilter = [](ETextureFilter _Filter)
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
      default:
        return GL_LINEAR_MIPMAP_LINEAR;
      }
    };

    glGenTextures(1, &m_ID);
    glBindTexture(m_Target, m_ID);
    glTexImage2D(m_Target, 0, GL_DEPTH_COMPONENT, _Params.Width, _Params.Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, ToGLWrap(_Params.WrapS));
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, ToGLWrap(_Params.WrapT));
    glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, ToGLFilter(_Params.MinFilter));
    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, ToGLFilter(_Params.MagFilter));

    if (_Params.BorderColors.has_value())
      glTexParameterfv(m_Target, GL_TEXTURE_BORDER_COLOR, _Params.BorderColors->Data());

    CLogger::Log(ELogType::Debug, "[CTexture] Texture generated successfully ({}x{})", _Params.Width, _Params.Height);
    return true;
  }
};

class CCubemap final : public CTextureBase
{
  friend CTextureBase;

  static constexpr inline int CUBEMAP_FACES = 6;

public:
  CCubemap() : CTextureBase(GL_TEXTURE_CUBE_MAP) {}

  bool Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>) override
  {
    assert(_Path.extension() == ".json");
    const nlohmann::json JsonContent = utils::ParseJson(_Path.string());
    const auto Faces = JsonContent["Faces"].get<std::vector<std::string>>();
    assert(Faces.size() == CUBEMAP_FACES);

    CStaticArray<TImage, CUBEMAP_FACES> Images;

    stbi_set_flip_vertically_on_load(false);
    for (int i = 0; i < CUBEMAP_FACES; ++i)
    {
      TImage Image;
      Image.Data = stbi_load((_Path.parent_path() / Faces[i]).c_str(), &Image.Width, &Image.Height, &Image.Channels, 0);

      if (Image.Data)
        Images.PushBack(std::move(Image));
    }
    stbi_set_flip_vertically_on_load(true);

    bool Success = false;
    if (Success = (Images.GetActualSize() == CUBEMAP_FACES))
    {
      glGenTextures(1, &m_ID);
      glBindTexture(m_Target, m_ID);

      for (int i = 0; i < Images.GetActualSize(); ++i)
      {
        const TImage &Image = Images[i];
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, Image.Width, Image.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Image.Data);
      }

      glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(m_Target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

      CLogger::Log(ELogType::Info, "[CCubemap] Texture '{}' loaded successfully", _Path.c_str());
    }
    else
    {
      CLogger::Log(ELogType::Error, "[CCubemap] Texture '{}' failed to load", _Path.c_str());
    }

    for (TImage &Image : Images)
      stbi_image_free(Image.Data);

    return Success;
  }

  bool Generate(const TTextureParams &_Params, CPasskey<CResourceManager>) override
  {
    (void)_Params;
    assert(false && "Cubemap generation isn't supported yet");
    return false;
  }
};