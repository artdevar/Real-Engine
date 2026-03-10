#include "../ComponentsFactory.h"

#include "assets/TextureParams.h"
#include "utils/Resource.h"

namespace ecs
{

void CComponentsFactory::CreateEnvironmentComponent(const std::filesystem::path &_Path, TEnvironmentComponent &_Component)
{
  {
    TTextureParams Params;
    Params.HDR               = true;
    Params.WrapS             = ETextureWrap::ClampToEdge;
    Params.WrapT             = ETextureWrap::ClampToEdge;
    Params.WrapR             = ETextureWrap::ClampToEdge;
    Params.MinFilter         = ETextureFilter::Linear;
    Params.MagFilter         = ETextureFilter::Linear;
    Params.InternalFormat    = GL_RGB16F;
    Params.Format            = GL_RGB;
    Params.Type              = GL_FLOAT;
    Params.Width             = 512;
    Params.Height            = 512;
    _Component.SkyboxTexture = resource::CreateCubemap("SKYBOX_CUBEMAP", Params);
  }

  {
    TTextureParams Params;
    Params.HDR               = true;
    Params.WrapS             = ETextureWrap::ClampToEdge;
    Params.WrapT             = ETextureWrap::ClampToEdge;
    Params.WrapR             = ETextureWrap::ClampToEdge;
    Params.MinFilter         = ETextureFilter::Linear;
    Params.MagFilter         = ETextureFilter::Linear;
    Params.InternalFormat    = GL_RGB16F;
    Params.Format            = GL_RGB;
    Params.Type              = GL_FLOAT;
    Params.Width             = 32;
    Params.Height            = 32;
    _Component.IrradianceMap = resource::CreateCubemap("IRRADIANCE_CUBEMAP", Params);
  }

  {
    TTextureParams Params;
    Params.HDR                    = true;
    Params.WrapS                  = ETextureWrap::ClampToEdge;
    Params.WrapT                  = ETextureWrap::ClampToEdge;
    Params.MinFilter              = ETextureFilter::Linear;
    Params.MagFilter              = ETextureFilter::Linear;
    _Component.EquirectangularMap = resource::LoadTexture(_Path, Params);
  }
}

} // namespace ecs