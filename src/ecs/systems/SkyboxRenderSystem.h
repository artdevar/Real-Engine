#pragma once

#include "ecs/SystemManager.h"
#include "ecs/CommonECS.h"
#include "interfaces/Renderable.h"
#include "graphics/Buffer.h"

class CShader;

namespace ecs
{

    class CSkyboxRenderSystem : public IRenderable,
                                public CSystem
    {
    public:
        CSkyboxRenderSystem();

        void Init(CCoordinator *_Coordinator) override;

    private:
        void RenderInternal(IRenderer &_Renderer) override;
        bool ShouldBeRendered() const override;

    protected:
        std::weak_ptr<CShader> m_SkyboxShader;
        CVertexArray m_VAO;
        CVertexBuffer m_VBO;
    };

}
