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
        void RenderInternal(IRenderer &_Renderer) override;
        bool ShouldBeRendered() const override;

        void RenderDebugQuad(IRenderer &_Renderer);

        void CreateDepthMap();

    public:
        static constexpr inline int SHADOW_MAP_SIZE = 2048;

        CFrameBuffer m_DepthMapFBO;
        std::shared_ptr<CTextureBase> m_DepthMap;
        std::shared_ptr<CShader> m_DepthShader;
    };

}
