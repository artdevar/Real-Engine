#include "../ComponentsFactory.h"

#include "physics/Collision.h"
#include "assets/Model.h"
#include "render/Buffer.h"
#include <glm/mat4x4.hpp>
#include <vector>

namespace ecs
{

static void CreateBoxGeometry(TCollisionComponent &_Component)
{
  static constexpr glm::vec3 Vertices[] = {// Bottom square
                                           {-0.5f, -0.5f, -0.5f},
                                           {0.5f, -0.5f, -0.5f},
                                           {0.5f, -0.5f, -0.5f},
                                           {0.5f, -0.5f, 0.5f},
                                           {0.5f, -0.5f, 0.5f},
                                           {-0.5f, -0.5f, 0.5f},
                                           {-0.5f, -0.5f, 0.5f},
                                           {-0.5f, -0.5f, -0.5f},

                                           // Top square
                                           {-0.5f, 0.5f, -0.5f},
                                           {0.5f, 0.5f, -0.5f},
                                           {0.5f, 0.5f, -0.5f},
                                           {0.5f, 0.5f, 0.5f},
                                           {0.5f, 0.5f, 0.5f},
                                           {-0.5f, 0.5f, 0.5f},
                                           {-0.5f, 0.5f, 0.5f},
                                           {-0.5f, 0.5f, -0.5f},

                                           // Vertical lines
                                           {-0.5f, -0.5f, -0.5f},
                                           {-0.5f, 0.5f, -0.5f},
                                           {0.5f, -0.5f, -0.5f},
                                           {0.5f, 0.5f, -0.5f},
                                           {0.5f, -0.5f, 0.5f},
                                           {0.5f, 0.5f, 0.5f},
                                           {-0.5f, -0.5f, 0.5f},
                                           {-0.5f, 0.5f, 0.5f}};

  _Component.VAO = std::make_shared<CVertexArray>();
  _Component.VBO = std::make_shared<CVertexBuffer>(GL_STATIC_DRAW);

  _Component.VAO->Bind();
  _Component.VBO->Bind();
  _Component.VBO->Assign(const_cast<glm::vec3 *>(Vertices), sizeof(Vertices));

  _Component.VAO->EnableAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

  _Component.VAO->Unbind();
  _Component.VBO->Unbind();
}

static glm::mat4 ComputeNodeTransformLocal(const TNode &_Node)
{
  if (std::holds_alternative<TMatrix>(_Node.TransformData))
    return std::get<TMatrix>(_Node.TransformData).Value;

  const TTRS &TRS         = std::get<TTRS>(_Node.TransformData);
  glm::mat4   Translation = glm::translate(glm::mat4(1.0f), TRS.Translation);
  glm::mat4   Scale       = glm::scale(glm::mat4(1.0f), TRS.Scale);
  glm::mat4   Rotation    = glm::mat4_cast(TRS.Rotation);
  return Translation * Rotation * Scale;
}

static void CalculateAABB(const TModelData &_Model, const TNode &_Node, TAABB &_AABB, const glm::mat4 &_ParentTransform)
{
  glm::mat4 WorldTransform = _ParentTransform * ComputeNodeTransformLocal(_Node);

  if (_Node.MeshIndex >= 0)
  {
    const TMesh &Mesh = _Model.Meshes[_Node.MeshIndex];
    for (const TPrimitive &Primitive : Mesh.Primitives)
    {
      if (Primitive.MinValues.has_value() && Primitive.MaxValues.has_value())
      {
        TAABB PrimitiveAABB{
            .Min = Primitive.MinValues.value(),
            .Max = Primitive.MaxValues.value(),
        };

        for (const glm::vec3 &Corner : PrimitiveAABB.GetCorners())
          _AABB.Expand(glm::vec3(WorldTransform * glm::vec4(Corner, 1.0f)));
      }
      else
      {
        auto It = Primitive.Attributes.find(EAttributeType::Position);
        if (It != Primitive.Attributes.end())
        {
          const TAttribute &PosAttr = It->second;
          const size_t      Count   = PosAttr.Data.size() / sizeof(glm::vec3);
          const auto       *Ptr     = reinterpret_cast<const glm::vec3 *>(PosAttr.Data.data());

          for (size_t i = 0; i < Count; ++i)
          {
            glm::vec4 WorldPos = WorldTransform * glm::vec4(Ptr[i], 1.0f);
            _AABB.Expand(glm::vec3(WorldPos));
          }
        }
      }
    }
  }

  for (int NodeChild : _Node.Children)
    CalculateAABB(_Model, _Model.Nodes[NodeChild], _AABB, WorldTransform);
}

void CComponentsFactory::CreateCollisionComponent(const std::shared_ptr<CModel> &_Model, TCollisionComponent &_Component)
{
  const TModelData &ModelData = _Model->GetModelData();

  for (int NodeIndex : ModelData.RootNodes)
    CalculateAABB(ModelData, ModelData.Nodes[NodeIndex], _Component.BoundingBox, glm::mat4(1.0f));

  CreateBoxGeometry(_Component);
}

} // namespace ecs
