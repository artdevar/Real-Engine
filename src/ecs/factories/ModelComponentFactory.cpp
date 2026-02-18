#include "../ComponentsFactory.h"

#include "render/Buffer.h"
#include "assets/Model.h"
#include "render/ShaderTypes.h"
#include "assets/Texture.h"
#include "utils/Resource.h"
#include "utils/Stopwatch.h"

namespace ecs
{
static GLint ToAttributeLocation(EAttributeType _Type)
{
  switch (_Type)
  {
  case EAttributeType::Position:
    return ATTRIB_LOC_POSITION;
  case EAttributeType::Normal:
    return ATTRIB_LOC_NORMAL;
  case EAttributeType::TexCoords_0:
    return ATTRIB_LOC_TEXCOORDS_0;
  case EAttributeType::TexCoords_1:
    return ATTRIB_LOC_TEXCOORDS_1;
  case EAttributeType::TexCoords_2:
    return ATTRIB_LOC_TEXCOORDS_2;
  case EAttributeType::TexCoords_3:
    return ATTRIB_LOC_TEXCOORDS_3;
  case EAttributeType::Tangent:
    return ATTRIB_LOC_TANGENT;
  default:
    assert(false && "Unknown attribute type");
    return -1;
  }
}

static GLenum ToRawAttributeComponentType(EAttributeComponentType _ComponentType)
{
  switch (_ComponentType)
  {
  case EAttributeComponentType::Byte:
    return GL_BYTE;
  case EAttributeComponentType::UnsignedByte:
    return GL_UNSIGNED_BYTE;
  case EAttributeComponentType::Short:
    return GL_SHORT;
  case EAttributeComponentType::UnsignedShort:
    return GL_UNSIGNED_SHORT;
  case EAttributeComponentType::Int:
    return GL_INT;
  case EAttributeComponentType::UnsignedInt:
    return GL_UNSIGNED_INT;
  case EAttributeComponentType::Float:
    return GL_FLOAT;
  default:
    assert(false && "Unknown attribute component type");
    return 0;
  }
}

static TTextureParams ParseSampler(const TSampler &_Sampler)
{
  TTextureParams Params;
  Params.WrapS     = _Sampler.WrapS;
  Params.WrapT     = _Sampler.WrapT;
  Params.MinFilter = _Sampler.MinFilter;
  Params.MagFilter = _Sampler.MagFilter;

  return Params;
}

static TModelComponent::TTexture LoadTexture(const TModelData &_ModelData, const TTexture &_Texture, ETextureType _TextureType)
{
  std::shared_ptr<CTextureBase> TexturePtr;
  if (_Texture.ImageIndex < 0 || _Texture.ImageIndex >= _ModelData.Images.size())
    TexturePtr = resource::GetDefaultTexture(_TextureType);
  else if (_Texture.SamplerIndex < 0 || _Texture.SamplerIndex >= _ModelData.Samplers.size())
    TexturePtr = resource::LoadTexture(_ModelData.Images[_Texture.ImageIndex].URI);
  else
    TexturePtr = resource::LoadTexture(_ModelData.Images[_Texture.ImageIndex].URI, ParseSampler(_ModelData.Samplers[_Texture.SamplerIndex]));

  assert(TexturePtr && "Failed to load texture");

  TModelComponent::TTexture Result;
  Result.Texture       = TexturePtr;
  Result.TexCoordIndex = _Texture.TexCoordIndex;

  return Result;
}

static void ParseMaterials(const TModelData &_ModelData, TModelComponent &_Component)
{
  _Component.Materials.reserve(_ModelData.Materials.size());
  for (const TMaterial &SrcMaterial : _ModelData.Materials)
  {
    TModelComponent::TMaterialData &Material = _Component.Materials.emplace_back();
    Material.BaseColorFactor                 = SrcMaterial.BaseColorFactor;
    Material.EmissiveFactor                  = SrcMaterial.EmissiveFactor;
    Material.MetallicFactor                  = SrcMaterial.MetallicFactor;
    Material.RoughnessFactor                 = SrcMaterial.RoughnessFactor;
    Material.IsDoubleSided                   = SrcMaterial.IsDoubleSided;
    Material.AlphaCutoff                     = SrcMaterial.AlphaCutoff;
    Material.AlphaMode                       = SrcMaterial.AlphaMode;
    Material.BaseColorTexture                = LoadTexture(_ModelData, SrcMaterial.BaseColorTexture, ETextureType::BasicColor);
    Material.MetallicRoughnessTexture        = LoadTexture(_ModelData, SrcMaterial.MetallicRoughnessTexture, ETextureType::Roughness);
    Material.NormalTexture                   = LoadTexture(_ModelData, SrcMaterial.NormalTexture, ETextureType::Normal);
    Material.EmissiveTexture                 = LoadTexture(_ModelData, SrcMaterial.EmissiveTexture, ETextureType::Emissive);
  }
}

static void ParseMesh(const TModelData &_Model, const TMesh &_Mesh, TModelComponent &_Component, const glm::mat4 &_NodeTransform)
{
  for (const TPrimitive &Primitive : _Mesh.Primitives)
  {
    TModelComponent::TPrimitiveData &PrimitiveData = _Component.Primitives.emplace_back();
    PrimitiveData.MaterialIndex                    = std::max(Primitive.MaterialIndex, 0);
    PrimitiveData.Mode                             = Primitive.Mode;
    PrimitiveData.PrimitiveMatrix                  = _NodeTransform;

    for (const auto &[Type, Attribute] : Primitive.Attributes)
    {
      const GLuint AttributeLoc = ToAttributeLocation(Type);

      assert(AttributeLoc != GLuint(-1));

      PrimitiveData.VAO.Bind();

      CVertexBuffer &VBO = PrimitiveData.VBOs.emplace_back(GL_STATIC_DRAW);
      VBO.Bind();
      VBO.Assign(Attribute.Data);

      PrimitiveData.VAO.EnableAttrib(AttributeLoc, Attribute.Type, ToRawAttributeComponentType(Attribute.ComponentType), Attribute.IsNormalized,
                                     Attribute.ByteStride);
      PrimitiveData.VAO.Unbind();
    }

    if (!Primitive.Indices.empty())
    {
      PrimitiveData.IndicesCount = Primitive.IndicesCount;
      PrimitiveData.Type         = Primitive.IndicesType;

      PrimitiveData.VAO.Bind();

      CElementBuffer &EBO = PrimitiveData.EBO.emplace(GL_STATIC_DRAW);
      EBO.Bind();
      EBO.Assign(Primitive.Indices);

      PrimitiveData.VAO.Unbind();
    }
    else
    {
      PrimitiveData.VerticesCount = Primitive.VerticesCount;
    }
  }
}

static glm::mat4 ComputeNodeTransform(const TNode &_Node)
{
  if (std::holds_alternative<TMatrix>(_Node.TransformData))
    return std::get<TMatrix>(_Node.TransformData).Value;

  const TTRS &TRS         = std::get<TTRS>(_Node.TransformData);
  glm::mat4   Translation = glm::translate(glm::mat4(1.0f), TRS.Translation);
  glm::mat4   Rotation    = glm::mat4_cast(TRS.Rotation);
  glm::mat4   Scale       = glm::scale(glm::mat4(1.0f), TRS.Scale);
  return Translation * Rotation * Scale;
}

static void ParseNodes(const TModelData &_Model, const TNode &_Node, TModelComponent &_Component, const glm::mat4 &_ParentTransform)
{
  glm::mat4 LocalTransform = ComputeNodeTransform(_Node);
  glm::mat4 WorldTransform = _ParentTransform * LocalTransform;

  if (_Node.MeshIndex >= 0)
    ParseMesh(_Model, _Model.Meshes[_Node.MeshIndex], _Component, WorldTransform);

  for (int NodeChild : _Node.Children)
  {
    assert(NodeChild >= 0 && NodeChild < _Model.Nodes.size());
    ParseNodes(_Model, _Model.Nodes[NodeChild], _Component, WorldTransform);
  }
}

void CComponentsFactory::CreateModelComponent(const std::shared_ptr<CModel> &_Model, TModelComponent &_Component)
{
  MEASURE_ZONE("CreateModelComponent");

  const TModelData &ModelData = _Model->GetModelData();

  if (ModelData.Materials.empty())
  {
    CLogger::Log(ELogType::Warning, "[CreateModelComponent] Model has no materials; creating a default material");

    TModelComponent::TMaterialData &Material  = _Component.Materials.emplace_back();
    Material.BaseColorFactor                  = glm::vec4(1.0f);
    Material.EmissiveFactor                   = glm::vec3(0.0f);
    Material.MetallicFactor                   = 1.0f;
    Material.RoughnessFactor                  = 1.0f;
    Material.IsDoubleSided                    = false;
    Material.AlphaCutoff                      = 0.5f;
    Material.AlphaMode                        = EAlphaMode::Opaque;
    Material.BaseColorTexture.Texture         = resource::GetDefaultTexture(ETextureType::BasicColor);
    Material.MetallicRoughnessTexture.Texture = resource::GetDefaultTexture(ETextureType::Roughness);
    Material.NormalTexture.Texture            = resource::GetDefaultTexture(ETextureType::Normal);
    Material.EmissiveTexture.Texture          = resource::GetDefaultTexture(ETextureType::Emissive);
  }
  else
  {
    ParseMaterials(ModelData, _Component);
  }

  for (int NodeIndex : ModelData.RootNodes)
  {
    assert(NodeIndex >= 0 && NodeIndex < ModelData.Nodes.size());
    ParseNodes(ModelData, ModelData.Nodes[NodeIndex], _Component, glm::mat4(1.0f));
  }
}

} // namespace ecs