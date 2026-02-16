#pragma once

#include "ecs/SystemManager.h"
#include "interfaces/Renderable.h"
#include "graphics/Buffer.h"
#include "engine/MathTypes.h"

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
        void PrepareRenderState(IRenderer &_Renderer, TVector2i _NewViewport);
        void RestoreRenderState(IRenderer &_Renderer, TVector2i _OldViewport);

        void RenderDebugQuad(IRenderer &_Renderer);

        void CreateDepthMap();

    public:
        const int SHADOW_MAP_SIZE;

        CFrameBuffer m_DepthMapFBO;
        std::weak_ptr<CShader> m_DepthShader;
        std::shared_ptr<CTextureBase> m_DepthMap;
    };

}
