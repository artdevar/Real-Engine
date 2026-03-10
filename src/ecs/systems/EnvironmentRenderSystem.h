#pragma once

#include "interfaces/RenderCollector.h"
#include "interfaces/FrameDataCollector.h"
#include <ecs/System.h>

namespace ecs
{

class CEnvironmentRenderSystem : public IRenderCollector,
                                 public IFrameDataCollector,
                                 public CSystem
{
public:
  void Collect(TFrameData &_FrameData) override;
  void Collect(CRenderQueue &_Queue) override;

protected:
  void OnEntityAdded(ecs::TEntity _Entity) override;

private:
  enum class EConversionStage
  {
    WaitingForEquirectangular, // Stage 0: Convert equirectangular → cubemap
    EquirectangularConverted,  // Stage 1: Convert cubemap → irradiance map
    IrradianceConverted,       // Stage 2: All done, ready to render
  };

  EConversionStage m_ConversionStage = EConversionStage::WaitingForEquirectangular;
};

} // namespace ecs
