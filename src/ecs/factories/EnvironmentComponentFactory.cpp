#include "../ComponentsFactory.h"

#include "assets/TextureParams.h"
#include "utils/Resource.h"

namespace ecs
{

void CComponentsFactory::CreateEnvironmentComponent(const std::filesystem::path &_Path, TEnvironmentComponent &_Component)
{
  {
    TTextureParams Params;
    Params.WrapS             = ETextureWrap::ClampToEdge;
    Params.WrapT             = ETextureWrap::ClampToEdge;
    Params.WrapR             = ETextureWrap::ClampToEdge;
    Params.MinFilter         = ETextureFilter::Linear;
    Params.MagFilter         = ETextureFilter::Linear;
    Params.InternalFormat    = EInternalFormat::RGB16F;
    Params.Width             = 512;
    Params.Height            = 512;
    Params.AllocateMipmaps   = true;
    _Component.SkyboxTexture = resource::CreateCubemap("SKYBOX_CUBEMAP", Params);
  }

  {
    TTextureParams Params;
    Params.WrapS             = ETextureWrap::ClampToEdge;
    Params.WrapT             = ETextureWrap::ClampToEdge;
    Params.WrapR             = ETextureWrap::ClampToEdge;
    Params.MinFilter         = ETextureFilter::Linear;
    Params.MagFilter         = ETextureFilter::Linear;
    Params.InternalFormat    = EInternalFormat::RGB16F;
    Params.Width             = 32;
    Params.Height            = 32;
    _Component.IrradianceMap = resource::CreateCubemap("IRRADIANCE_CUBEMAP", Params);
  }

  {
    TTextureParams Params;
    Params.WrapS              = ETextureWrap::ClampToEdge;
    Params.WrapT              = ETextureWrap::ClampToEdge;
    Params.WrapR              = ETextureWrap::ClampToEdge;
    Params.MinFilter          = ETextureFilter::LinearMipmapLinear;
    Params.MagFilter          = ETextureFilter::Linear;
    Params.InternalFormat     = EInternalFormat::RGB16F;
    Params.Width              = 128;
    Params.Height             = 128;
    Params.MipmapsCount       = 5;
    _Component.PrefilteredMap = resource::CreateCubemap("PREFILTERED_CUBEMAP", Params);
  }

  {
    TTextureParams Params;
    Params.WrapS          = ETextureWrap::ClampToEdge;
    Params.WrapT          = ETextureWrap::ClampToEdge;
    Params.WrapR          = ETextureWrap::ClampToEdge;
    Params.MinFilter      = ETextureFilter::Linear;
    Params.MagFilter      = ETextureFilter::Linear;
    Params.InternalFormat = EInternalFormat::RG16F;
    Params.Format         = EFormat::RG;
    Params.Type           = EType::Float;
    Params.Width          = 512;
    Params.Height         = 512;
    _Component.BRDFLUT    = resource::CreateTexture("BRDFLUT_TEXTURE", Params);
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
