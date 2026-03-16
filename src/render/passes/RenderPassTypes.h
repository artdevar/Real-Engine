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
  TAA
};