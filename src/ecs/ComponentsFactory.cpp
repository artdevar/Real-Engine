#include "ComponentsFactory.h"
#include "graphics/Model.h"
#include "graphics/ShaderTypes.h"
#include "graphics/Texture.h"
#include "utils/Stopwatch.h"
#include "utils/Resource.h"
#include <glad/glad.h>

namespace ecs
{
  static EAlphaMode GetAlphaMode(EModelAlphaMode _ModelAlphaMode)
  {
    switch (_ModelAlphaMode)
    {
    case EModelAlphaMode::Mask:
      return EAlphaMode::Mask;
    case EModelAlphaMode::Blend:
      return EAlphaMode::Blend;
    default:
      return EAlphaMode::Opaque;
    }
  }

  static ETextureWrap ToTextureWrap(ETextureWrapMode _Wrap)
  {
    switch (_Wrap)
    {
    case ETextureWrapMode::Repeat:
      return ETextureWrap::Repeat;
    case ETextureWrapMode::ClampToEdge:
      return ETextureWrap::ClampToEdge;
    case ETextureWrapMode::ClampToBorder:
      return ETextureWrap::ClampToBorder;
    case ETextureWrapMode::MirroredRepeat:
      return ETextureWrap::MirroredRepeat;
    default:
      assert(false && "Unknown wrap mode");
      return ETextureWrap::Repeat;
    }
  }

  static ETextureFilter ToTextureFilter(ETextureFilterMode _Filter)
  {
    switch (_Filter)
    {
    case ETextureFilterMode::Nearest:
      return ETextureFilter::Nearest;
    case ETextureFilterMode::Linear:
      return ETextureFilter::Linear;
    case ETextureFilterMode::LinearMipmapLinear:
      return ETextureFilter::LinearMipmapLinear;
    case ETextureFilterMode::LinearMipmapNearest:
      return ETextureFilter::LinearMipmapNearest;
    case ETextureFilterMode::NearestMipmapNearest:
      return ETextureFilter::NearestMipmapNearest;
    case ETextureFilterMode::NearestMipmapLinear:
      return ETextureFilter::NearestMipmapLinear;
    default:
      assert(false && "Unknown filter mode");
      return ETextureFilter::LinearMipmapLinear;
    }
  }

  static TTextureParams ParseSampler(const TSampler &_Sampler)
  {
    TTextureParams Params;
    Params.WrapS = ToTextureWrap(_Sampler.WrapS);
    Params.WrapT = ToTextureWrap(_Sampler.WrapT);
    Params.MinFilter = ToTextureFilter(_Sampler.MinFilter);
    Params.MagFilter = ToTextureFilter(_Sampler.MagFilter);

    return Params;
  }

  static std::shared_ptr<CTextureBase> LoadTexture(const TModelData &_ModelData, const TTexture &_Texture, ETextureType _TextureType)
  {
    if (_Texture.ImageIndex < 0 || _Texture.ImageIndex >= _ModelData.Images.size())
      return resource::GetDefaultTexture(_TextureType);

    if (_Texture.SamplerIndex < 0 || _Texture.SamplerIndex >= _ModelData.Samplers.size())
      return resource::LoadTexture(_ModelData.Images[_Texture.ImageIndex].URI);
    else
      return resource::LoadTexture(_ModelData.Images[_Texture.ImageIndex].URI, ParseSampler(_ModelData.Samplers[_Texture.SamplerIndex]));
  }

  static void ParseMaterials(const TModelData &_ModelData, TModelComponent &_Component)
  {
    _Component.Materials.reserve(_ModelData.Materials.size());
    for (const TMaterial &SrcMaterial : _ModelData.Materials)
    {
      TModelComponent::TMaterialData &Material = _Component.Materials.emplace_back();
      Material.BaseColorFactor = SrcMaterial.BaseColorFactor;
      Material.EmissiveFactor = SrcMaterial.EmissiveFactor;
      Material.MetallicFactor = SrcMaterial.MetallicFactor;
      Material.RoughnessFactor = SrcMaterial.RoughnessFactor;
      Material.IsDoubleSided = SrcMaterial.IsDoubleSided;
      Material.AlphaCutoff = SrcMaterial.AlphaCutoff;
      Material.AlphaMode = GetAlphaMode(SrcMaterial.AlphaMode);
      Material.BaseColorTexture = LoadTexture(_ModelData, SrcMaterial.BaseColorTexture, ETextureType::BasicColor);
      Material.MetallicRoughnessTexture = LoadTexture(_ModelData, SrcMaterial.MetallicRoughnessTexture, ETextureType::Roughness);
      Material.NormalTexture = LoadTexture(_ModelData, SrcMaterial.NormalTexture, ETextureType::Normal);
      Material.EmissiveTexture = LoadTexture(_ModelData, SrcMaterial.EmissiveTexture, ETextureType::Emissive);
    }
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
                                                                                         : Type == EAttributeType::Tangent   ? ATTRIB_LOC_TANGENT
                                                                                                                             : GLuint(-1);

        assert(AttributeLoc != GLuint(-1));

        PrimitiveData.VAO.Bind();

        CVertexBuffer VBO(GL_STATIC_DRAW);
        VBO.Bind();
        VBO.Assign(Attribute.Data);

        PrimitiveData.VAO.EnableAttrib(AttributeLoc, Attribute.Type, Attribute.ComponentType, Attribute.ByteStride);
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

  static void SortPrimitives(TModelComponent &_Component)
  {
    std::stable_sort(
        _Component.Primitives.begin(),
        _Component.Primitives.end(),
        [&_Component](const TModelComponent::TPrimitiveData &A, const TModelComponent::TPrimitiveData &B)
        {
          auto getAlphaMode = [&_Component](int idx) -> EAlphaMode
          {
            if (idx >= 0 && idx < static_cast<int>(_Component.Materials.size()))
              return _Component.Materials[idx].AlphaMode;
            return EAlphaMode::Opaque;
          };

          const EAlphaMode AMode = getAlphaMode(A.MaterialIndex);
          const EAlphaMode BMode = getAlphaMode(B.MaterialIndex);

          // Only BLEND materials go to the back
          const bool AIsBlend = (AMode == EAlphaMode::Blend);
          const bool BIsBlend = (BMode == EAlphaMode::Blend);

          if (AIsBlend != BIsBlend)
            return !AIsBlend;

          return false;
        });
  }

  void CComponentsFactory::CreateModelComponent(const std::shared_ptr<CModel> &_Model, TModelComponent &_Component)
  {
    MEASURE_ZONE("CreateModelComponent");

    const TModelData &ModelData = _Model->GetModelData();

    assert(!ModelData.Materials.empty() && "Model should have at least one material. Handle this case by adding a default material if needed.");
    ParseMaterials(ModelData, _Component);

    for (int NodeIndex : ModelData.RootNodes)
    {
      assert(NodeIndex >= 0 && NodeIndex < ModelData.Nodes.size());
      ParseNodes(ModelData, ModelData.Nodes[NodeIndex], _Component);
    }

    SortPrimitives(_Component);
  }

  void CComponentsFactory::CreateSkyboxComponent(const std::shared_ptr<CTextureBase> &_Skybox, TSkyboxComponent &_Component)
  {
    _Component.SkyboxTexture = _Skybox;
  }
}