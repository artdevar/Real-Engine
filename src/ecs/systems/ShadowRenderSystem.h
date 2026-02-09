#pragma once

#include "ecs/SystemManager.h"
#include "interfaces/Renderable.h"
#include "graphics/Buffer.h"

class CTextureBase;
class CShader;

namespace ecs
{

    class CShadowRenderSystem : public IRenderable,
                                public CSystem
    {
    public:
        CShadowRenderSystem();

        void Init(CCoordinator *_Coordinator) override;

    private:
        void RenderInternal(CRenderer &_Renderer) override;
        bool ShouldBeRendered() const override;

        void RenderDebugQuad(CRenderer &_Renderer);

    public:
        static constexpr inline int SHADOW_MAP_SIZE = 1024;

        CFrameBuffer m_DepthMapFBO;
        std::shared_ptr<CTextureBase> m_DepthMap;
        std::shared_ptr<CShader> m_DepthShader;
    };

}
