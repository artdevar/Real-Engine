#include "ecs/SystemManager.h"
#include "interfaces/RenderCollector.h"

namespace ecs
{

class CModelRenderSystem : public IRenderCollector,
                           public CSystem
{
public:
  void Collect(CRenderQueue &_Queue) override;

  void SetVisibility(ecs::TEntity _Entity, bool _IsVisible);

private:
  void OnEntityAdded(ecs::TEntity _Entity) override;

  void OnEntityDeleted(ecs::TEntity _Entity) override;

protected:
  CUnorderedVector<ecs::TEntity> m_HiddenEntities;
};

} // namespace ecs