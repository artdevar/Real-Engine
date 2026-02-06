#include "ComponentsFactory.h"
#include "graphics/Model.h"
#include "graphics/ShaderTypes.h"
#include "graphics/Texture.h"
#include "utils/Stopwatch.h"
#include "utils/Resource.h"
#include <glad/glad.h>

namespace ecs
{
  static std::shared_ptr<CTextureBase> LoadTexture(const TModelData &_ModelData, int _ImageIndex)
  {
    if (_ImageIndex < 0 || _ImageIndex >= _ModelData.Images.size())
      return nullptr;

    return resource::LoadTexture(_ModelData.Images[_ImageIndex].URI);
  }

  static void ParseMesh(const TModelData &_Model, const TMesh &_Mesh, TModelComponent &_Component)
  {
    for (const TPrimitive &Primitive : _Mesh.Primitives)
    {
      TModelComponent::TPrimitiveData &PrimitiveData = _Component.Primitives.emplace_back();
      PrimitiveData.MaterialIndex = Primitive.MaterialIndex;

      for (const auto &[Type, Attribute] : Primitive.Attributes)
      {
        const GLuint AttributeLoc = Type == EAttributeType::Position ? ATTRIB_LOC_POSITION : Type == EAttributeType::Normal  ? ATTRIB_LOC_NORMAL
                                                                                         : Type == EAttributeType::TexCoords ? ATTRIB_LOC_TEXCOORDS
                                                                                                                             : GLuint(-1);

        assert(AttributeLoc != GLuint(-1));

        PrimitiveData.VAO.Bind();

        CVertexBuffer VBO(GL_STATIC_DRAW);
        VBO.Bind();
        VBO.Assign(Attribute.Data);

        const int ComponentCount = Type == EAttributeType::Position ? 3 : Type == EAttributeType::Normal  ? 3
                                                                      : Type == EAttributeType::TexCoords ? 2
                                                                                                          : 0;

        PrimitiveData.VAO.EnableAttrib(AttributeLoc, ComponentCount, Attribute.ComponentType, Attribute.ByteStride, (void *)0);
        PrimitiveData.VAO.Unbind();
      }

      if (!Primitive.Indices.empty())
      {
        PrimitiveData.Indices = Primitive.IndicesCount;
        PrimitiveData.Offset = 0;

        PrimitiveData.VAO.Bind();

        CElementBuffer EBO(GL_STATIC_DRAW);
        EBO.Bind();
        EBO.Assign(Primitive.Indices);

        PrimitiveData.VAO.Unbind();
      }
    }
  }

  static void ParseNodes(const TModelData &_Model, const TNode &_Node, TModelComponent &_Component)
  {
    if (_Node.MeshIndex >= 0)
      ParseMesh(_Model, _Model.Meshes[_Node.MeshIndex], _Component);

    for (int NodeChild : _Node.Children)
    {
      assert(NodeChild >= 0 && NodeChild < _Model.Nodes.size());
      ParseNodes(_Model, _Model.Nodes[NodeChild], _Component);
    }
  }

  void CComponentsFactory::CreateModelComponent(const std::shared_ptr<CModel> &_Model, TModelComponent &_Component)
  {
    MEASURE_ZONE("CreateModelComponent");

    const TModelData &ModelData = _Model->GetModelData();

    _Component.Materials.reserve(ModelData.Materials.size());
    for (const TMaterial &SrcMaterial : ModelData.Materials)
    {
      TModelComponent::TMaterialData &Material = _Component.Materials.emplace_back();
      Material.BaseColorFactor = SrcMaterial.BaseColorFactor;
      Material.MetallicFactor = SrcMaterial.MetallicFactor;
      Material.RoughnessFactor = SrcMaterial.RoughnessFactor;

      Material.BaseColorTexture = LoadTexture(ModelData, SrcMaterial.BaseColorTextureIndex);
      Material.MetallicRoughnessTexture = LoadTexture(ModelData, SrcMaterial.MetallicRoughnessTextureIndex);
    }

    for (int NodeIndex : ModelData.RootNodes)
    {
      assert(NodeIndex >= 0 && NodeIndex < ModelData.Nodes.size());
      ParseNodes(ModelData, ModelData.Nodes[NodeIndex], _Component);
    }
  }

  void CComponentsFactory::CreateSkyboxComponent(const std::shared_ptr<CTextureBase> &_Skybox, TSkyboxComponent &_Component)
  {
    _Component.TextureUnit = _Skybox->Get();
  }

}