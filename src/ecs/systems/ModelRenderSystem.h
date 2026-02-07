#include "ecs/SystemManager.h"
#include "interfaces/Renderable.h"
#include "interfaces/Updateable.h"
#include "interfaces/Shutdownable.h"
#include "graphics/Buffer.h"

struct TShaderLighting;
class CShader;

namespace ecs
{

    class CModelRenderSystem : public IRenderable,
                               public CSystem
    {
    public:
        CModelRenderSystem();

        void Init(CCoordinator *_Coordinator) override;

        void Render(CRenderer &_Renderer) override;

        void SetVisibility(ecs::TEntity _Entity, bool _IsVisible);

    protected:
        void OnEntityAdded(ecs::TEntity _Entity) override;

        void OnEntityDeleted(ecs::TEntity _Entity) override;

    protected:
        std::weak_ptr<CShader> m_ModelShader;
        CUnorderedVector<ecs::TEntity> m_HiddenEntities;
    };

}