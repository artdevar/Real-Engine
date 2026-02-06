#pragma once

#include <glad/glad.h>
#include <nlohmann/json.hpp>
#include "interfaces/Asset.h"
#include "utils/Common.h"
#include "utils/Logger.h"
#include "utils/Json.h"
#include "Shared.h"
#include "stb_image.h"

class CTextureBase : public IAsset
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

  bool Load(const std::filesystem::path &_Path) override
  {
    shared::TImage Image;
    Image.Data = stbi_load(_Path.c_str(), &Image.Width, &Image.Height, &Image.Channels, 0);

    if (!Image.Data)
    {
      CLogger::Log(ELogType::Error, std::format("Texture '{}' failed to load", _Path.c_str()));
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
    glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(Image.Data);

    CLogger::Log(ELogType::Info, std::format("Texture '{}' loaded successfully", _Path.c_str()));

    return true;
  }
};

class CCubemap final : public CTextureBase
{
  friend CTextureBase;

public:
  CCubemap() : CTextureBase(GL_TEXTURE_CUBE_MAP) {}

  bool Load(const std::filesystem::path &_Path) override
  {
    assert(_Path.extension() == ".json");
    const nlohmann::json JsonContent = utils::ParseJson(_Path.string());
    const auto Faces = JsonContent["Faces"].get<std::vector<std::string>>();
    assert(Faces.size() == shared::CUBEMAP_FACES);

    CStaticArray<shared::TImage, shared::CUBEMAP_FACES> Images;

    stbi_set_flip_vertically_on_load(false);
    for (int i = 0; i < shared::CUBEMAP_FACES; ++i)
    {
      shared::TImage Image;
      Image.Data = stbi_load((_Path.parent_path() / Faces[i]).c_str(), &Image.Width, &Image.Height, &Image.Channels, 0);

      if (Image.Data)
        Images.PushBack(std::move(Image));
    }
    stbi_set_flip_vertically_on_load(true);

    bool Success = false;
    if (Success = (Images.GetActualSize() == shared::CUBEMAP_FACES))
    {
      glGenTextures(1, &m_ID);
      glBindTexture(m_Target, m_ID);

      for (int i = 0; i < Images.GetActualSize(); ++i)
      {
        const shared::TImage &Image = Images[i];
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, Image.Width, Image.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Image.Data);
      }

      glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(m_Target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

      CLogger::Log(ELogType::Info, std::format("Texture '{}' loaded successfully", _Path.c_str()));
    }
    else
    {
      CLogger::Log(ELogType::Error, std::format("Texture '{}' failed to load", _Path.c_str()));
    }

    for (shared::TImage &Image : Images)
      stbi_image_free(Image.Data);

    return Success;
  }
};