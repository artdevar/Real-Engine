#pragma once

enum class ERenderPassType
{
  Opaque,
  Transparent,
  Shadow,
  Skybox,
  PostProcess,
  Collision,
  Grid,
  Output,
  EquirectangularToCubemap,
  IrradianceConvolution,
  Bloom,
  TAA,

  Common_Utility = 1000,
  Common_Shadow,
  Common_Geometry,
  Common_PostProcess,
  Common_Debug,
  Common_Output
};
