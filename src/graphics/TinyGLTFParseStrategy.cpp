#include "pch.h"

#include "TinyGLTFParseStrategy.h"
#include "utils/Logger.h"
#include "utils/Stopwatch.h"
#include <tiny_gltf.h>
#include <mikktspace.h>

namespace
{

  EAlphaMode ToModelAlphaMode(const std::string &_Mode)
  {
    if (_Mode == "MASK")
      return EAlphaMode::Mask;
    else if (_Mode == "BLEND")
      return EAlphaMode::Blend;
    else
      return EAlphaMode::Opaque;
  }

  ETextureWrap ToTextureWrapMode(int _Wrap)
  {
    switch (_Wrap)
    {
    case TINYGLTF_TEXTURE_WRAP_REPEAT:
      return ETextureWrap::Repeat;
    case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
      return ETextureWrap::ClampToEdge;
    case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
      return ETextureWrap::MirroredRepeat;
    default:
      return ETextureWrap::Repeat;
    }
  }

  ETextureFilter ToTextureFilterMode(int _Filter)
  {
    switch (_Filter)
    {
    case TINYGLTF_TEXTURE_FILTER_NEAREST:
      return ETextureFilter::Nearest;
    case TINYGLTF_TEXTURE_FILTER_LINEAR:
      return ETextureFilter::Linear;
    case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
      return ETextureFilter::NearestMipmapNearest;
    case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
      return ETextureFilter::LinearMipmapNearest;
    case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
      return ETextureFilter::NearestMipmapLinear;
    case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
      return ETextureFilter::LinearMipmapLinear;
    default:
      return ETextureFilter::Nearest;
    }
  }

  EAttributeType ToAttributeType(const std::string &_Name)
  {
    if (_Name == "POSITION")
      return EAttributeType::Position;
    else if (_Name == "NORMAL")
      return EAttributeType::Normal;
    else if (_Name == "TEXCOORD_0")
      return EAttributeType::TexCoords_0;
    else if (_Name == "TEXCOORD_1")
      return EAttributeType::TexCoords_1;
    else if (_Name == "TEXCOORD_2")
      return EAttributeType::TexCoords_2;
    else if (_Name == "TEXCOORD_3")
      return EAttributeType::TexCoords_3;
    else if (_Name == "TANGENT")
      return EAttributeType::Tangent;
    else
      return static_cast<EAttributeType>(-1);
  }

  EPrimitiveMode ToPrimitiveRenderMode(int _Mode)
  {
    switch (_Mode)
    {
    case TINYGLTF_MODE_POINTS:
      return EPrimitiveMode::Points;
    case TINYGLTF_MODE_LINE:
      return EPrimitiveMode::Line;
    case TINYGLTF_MODE_LINE_LOOP:
      return EPrimitiveMode::LineLoop;
    case TINYGLTF_MODE_LINE_STRIP:
      return EPrimitiveMode::LineStrip;
    case TINYGLTF_MODE_TRIANGLES:
      return EPrimitiveMode::Triangles;
    case TINYGLTF_MODE_TRIANGLE_STRIP:
      return EPrimitiveMode::TriangleStrip;
    case TINYGLTF_MODE_TRIANGLE_FAN:
      return EPrimitiveMode::TriangleFan;
    default:
      return EPrimitiveMode::Triangles;
    }
  }

  EAttributeComponentType ToAttributeComponentType(int _ComponentType)
  {
    switch (_ComponentType)
    {
    case TINYGLTF_COMPONENT_TYPE_BYTE:
      return EAttributeComponentType::Byte;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
      return EAttributeComponentType::UnsignedByte;
    case TINYGLTF_COMPONENT_TYPE_SHORT:
      return EAttributeComponentType::Short;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
      return EAttributeComponentType::UnsignedShort;
    case TINYGLTF_COMPONENT_TYPE_INT:
      return EAttributeComponentType::Int;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
      return EAttributeComponentType::UnsignedInt;
    case TINYGLTF_COMPONENT_TYPE_FLOAT:
      return EAttributeComponentType::Float;
    default:
      return EAttributeComponentType::Float;
    }
  }

  EIndexType ToIndexType(int _ComponentType)
  {
    switch (_ComponentType)
    {
    case TINYGLTF_COMPONENT_TYPE_BYTE:
      return EIndexType::Byte;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
      return EIndexType::UnsignedByte;
    case TINYGLTF_COMPONENT_TYPE_SHORT:
      return EIndexType::Short;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
      return EIndexType::UnsignedShort;
    case TINYGLTF_COMPONENT_TYPE_INT:
      return EIndexType::Int;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
      return EIndexType::UnsignedInt;
    case TINYGLTF_COMPONENT_TYPE_FLOAT:
      return EIndexType::Float;
    default:
      return EIndexType::UnsignedInt;
    }
  }

  static void CopyData(const std::vector<uint8_t> &_Src, std::vector<uint8_t> &_Dst, size_t _Offset, size_t _Count, size_t _Stride, size_t _ElementSize)
  {
    _Dst.reserve(_Dst.size() + _Count * _ElementSize);

    for (size_t i = 0; i < _Count; ++i)
    {
      const size_t SrcIndex = _Offset + i * _Stride;
      std::copy(_Src.begin() + SrcIndex, _Src.begin() + SrcIndex + _ElementSize, std::back_inserter(_Dst));
    }
  }
}

bool CTinyGLTFParseStrategy::Parse(const std::filesystem::path &_Path, TModelData &_Model)
{
  tinygltf::Model NativeModel;
  tinygltf::TinyGLTF Loader;
  std::string Error, Warning;

  const bool IsLoaded = Loader.LoadASCIIFromFile(&NativeModel, &Error, &Warning, _Path.string());

  if (!Warning.empty())
    CLogger::Log(ELogType::Warning, "Model loading {} warning: {}", _Path.string(), Warning);

  if (!Error.empty())
    CLogger::Log(ELogType::Error, "Model loading {} error: {}", _Path.string(), Error);

  if (!IsLoaded)
  {
    CLogger::Log(ELogType::Error, "Model loading {} failed", _Path.string());
    return false;
  }

  CLogger::Log(ELogType::Info, "Model {} loaded successfully", _Path.string());

  {
    MEASURE_ZONE("Parse GLTF Model");
    ParseModel(NativeModel, _Model, _Path.parent_path());
  }

  return true;
}

void CTinyGLTFParseStrategy::ParseModel(const tinygltf::Model &_Source, TModelData &_Target, const std::filesystem::path &_ModelDirectory)
{
  ParseImages(_Source, _Target, _ModelDirectory);
  ParseMaterials(_Source, _Target);
  ParseMeshes(_Source, _Target);
  ParseNodes(_Source, _Target);
  ParseScene(_Source, _Target);
}

void CTinyGLTFParseStrategy::ParseScene(const tinygltf::Model &_Source, TModelData &_Target)
{
  const tinygltf::Scene &Scene = _Source.scenes[_Source.defaultScene];
  _Target.RootNodes.reserve(Scene.nodes.size());
  _Target.RootNodes.insert(_Target.RootNodes.end(), Scene.nodes.begin(), Scene.nodes.end());
}

void CTinyGLTFParseStrategy::ParseNodes(const tinygltf::Model &_Source, TModelData &_Target)
{
  _Target.Nodes.reserve(_Source.nodes.size());

  for (const tinygltf::Node &SourceNode : _Source.nodes)
  {
    TNode &Node = _Target.Nodes.emplace_back();
    Node.MeshIndex = SourceNode.mesh;
    Node.Children.reserve(SourceNode.children.size());
    Node.Children.insert(Node.Children.end(), SourceNode.children.begin(), SourceNode.children.end());

    if (!SourceNode.matrix.empty())
    {
      Node.TransformData = TMatrix{glm::make_mat4(SourceNode.matrix.data())};
      continue;
    }

    glm::vec3 Translation = glm::vec3(0.0f);
    if (!SourceNode.translation.empty())
      Translation = glm::make_vec3(SourceNode.translation.data());

    glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    if (!SourceNode.rotation.empty())
      Rotation = glm::make_quat(SourceNode.rotation.data());

    glm::vec3 Scale = glm::vec3(1.0f);
    if (!SourceNode.scale.empty())
      Scale = glm::make_vec3(SourceNode.scale.data());

    Node.TransformData = TTRS{Translation, Rotation, Scale};
  }
}

void CTinyGLTFParseStrategy::ParseMeshes(const tinygltf::Model &_Source, TModelData &_Target)
{
  _Target.Meshes.reserve(_Source.meshes.size());

  for (const tinygltf::Mesh &SourceMesh : _Source.meshes)
  {
    TMesh &Mesh = _Target.Meshes.emplace_back();
    Mesh.Primitives.reserve(SourceMesh.primitives.size());

    for (const tinygltf::Primitive &SourcePrimitive : SourceMesh.primitives)
    {
      TPrimitive &Primitive = Mesh.Primitives.emplace_back();
      Primitive.Mode = ToPrimitiveRenderMode(SourcePrimitive.mode);
      Primitive.MaterialIndex = SourcePrimitive.material;

      ParseAttributes(_Source, SourcePrimitive, Primitive);
      ParseIndices(_Source, SourcePrimitive, Primitive);

      GenerateTangentsIfMissing(Primitive);
    }
  }
}

void CTinyGLTFParseStrategy::ParseAttributes(const tinygltf::Model &_Source, const tinygltf::Primitive &_SourcePrimitive, TPrimitive &_TargetPrimitive)
{
  assert(_SourcePrimitive.attributes.contains("POSITION") && "POSITION attribute is missing");

  for (const auto &[Name, AccessorIndex] : _SourcePrimitive.attributes)
  {
    const EAttributeType Type = ToAttributeType(Name);
    if (Type == static_cast<EAttributeType>(-1))
    {
      CLogger::Log(ELogType::Debug, "Unsupported attribute type: {}", Name);
      continue;
    }

    const tinygltf::Accessor &Accessor = _Source.accessors[AccessorIndex];
    const tinygltf::BufferView &BufferView = _Source.bufferViews[Accessor.bufferView];
    const tinygltf::Buffer &Buffer = _Source.buffers[BufferView.buffer];

    const size_t Offset = BufferView.byteOffset + Accessor.byteOffset;
    const size_t Stride = Accessor.ByteStride(BufferView);
    const size_t ComponentSize = tinygltf::GetComponentSizeInBytes(Accessor.componentType);
    const size_t ComponentCount = tinygltf::GetNumComponentsInType(Accessor.type);
    const size_t ElementSize = ComponentSize * ComponentCount;

    if (Accessor.sparse.isSparse)
    {
      CLogger::Log(ELogType::Warning, "Sparse accessors are not supported (attribute: {})", Name);
      continue;
    }

    if (Type == EAttributeType::Position)
      _TargetPrimitive.VerticesCount = static_cast<uint32_t>(Accessor.count);

    TAttribute &Attribute = _TargetPrimitive.Attributes[Type];
    Attribute.ComponentType = ToAttributeComponentType(Accessor.componentType);
    Attribute.ByteStride = Stride;
    Attribute.Type = Accessor.type;
    CopyData(Buffer.data, Attribute.Data, Offset, Accessor.count, Stride, ElementSize);
  }
}

void CTinyGLTFParseStrategy::ParseIndices(const tinygltf::Model &_Source, const tinygltf::Primitive &_SourcePrimitive, TPrimitive &_TargetPrimitive)
{
  if (_SourcePrimitive.indices < 0)
  {
    CLogger::Log(ELogType::Debug, "Primitive has no indices!");
    return;
  }

  const tinygltf::Accessor &Accessor = _Source.accessors[_SourcePrimitive.indices];
  const tinygltf::BufferView &BufferView = _Source.bufferViews[Accessor.bufferView];
  const tinygltf::Buffer &Buffer = _Source.buffers[BufferView.buffer];

  assert(Accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT && "Really?");

  _TargetPrimitive.IndicesCount = Accessor.count;
  _TargetPrimitive.IndicesType = ToIndexType(Accessor.componentType);

  const size_t Offset = BufferView.byteOffset + Accessor.byteOffset;
  const size_t Stride = Accessor.ByteStride(BufferView);
  const size_t ComponentSize = tinygltf::GetComponentSizeInBytes(Accessor.componentType);
  const size_t ComponentCount = tinygltf::GetNumComponentsInType(Accessor.type);
  const size_t ElementSize = ComponentSize * ComponentCount;

  CopyData(Buffer.data, _TargetPrimitive.Indices, Offset, Accessor.count, Stride, ElementSize);
}

void CTinyGLTFParseStrategy::ParseMaterials(const tinygltf::Model &_Source, TModelData &_Target)
{
  _Target.Materials.reserve(_Source.materials.size());

  for (const tinygltf::Material &SourceMaterial : _Source.materials)
  {
    TMaterial &Material = _Target.Materials.emplace_back();
    const tinygltf::PbrMetallicRoughness &PBR = SourceMaterial.pbrMetallicRoughness;

    if (PBR.baseColorTexture.index >= 0)
    {
      Material.BaseColorTexture.ImageIndex = PBR.baseColorTexture.index;
      Material.BaseColorTexture.TexCoordIndex = PBR.baseColorTexture.texCoord;
      Material.BaseColorTexture.SamplerIndex = _Source.textures[PBR.baseColorTexture.index].sampler;
    }

    if (PBR.metallicRoughnessTexture.index >= 0)
    {
      Material.MetallicRoughnessTexture.ImageIndex = PBR.metallicRoughnessTexture.index;
      Material.MetallicRoughnessTexture.TexCoordIndex = PBR.metallicRoughnessTexture.texCoord;
      Material.MetallicRoughnessTexture.SamplerIndex = _Source.textures[PBR.metallicRoughnessTexture.index].sampler;
    }

    if (SourceMaterial.normalTexture.index >= 0)
    {
      Material.NormalTexture.ImageIndex = SourceMaterial.normalTexture.index;
      Material.NormalTexture.TexCoordIndex = SourceMaterial.normalTexture.texCoord;
      Material.NormalTexture.SamplerIndex = _Source.textures[SourceMaterial.normalTexture.index].sampler;
    }

    if (SourceMaterial.emissiveTexture.index >= 0)
    {
      Material.EmissiveTexture.ImageIndex = SourceMaterial.emissiveTexture.index;
      Material.EmissiveTexture.TexCoordIndex = SourceMaterial.emissiveTexture.texCoord;
      Material.EmissiveTexture.SamplerIndex = _Source.textures[SourceMaterial.emissiveTexture.index].sampler;
    }

    const std::vector<double> &Factor = PBR.baseColorFactor;
    const std::vector<double> &EmissiveFactor = SourceMaterial.emissiveFactor;
    Material.BaseColorFactor = glm::vec4(Factor[0], Factor[1], Factor[2], Factor[3]);
    Material.EmissiveFactor = glm::vec3(EmissiveFactor[0], EmissiveFactor[1], EmissiveFactor[2]);
    Material.MetallicFactor = static_cast<float>(PBR.metallicFactor);
    Material.RoughnessFactor = static_cast<float>(PBR.roughnessFactor);
    Material.AlphaCutoff = static_cast<float>(SourceMaterial.alphaCutoff);
    Material.AlphaMode = ToModelAlphaMode(SourceMaterial.alphaMode);
    Material.IsDoubleSided = SourceMaterial.doubleSided;
  }
}

void CTinyGLTFParseStrategy::ParseImages(const tinygltf::Model &_Source, TModelData &_Target, const std::filesystem::path &_ModelDirectory)
{
  _Target.Images.reserve(_Source.images.size());

  for (const tinygltf::Image &SourceImage : _Source.images)
  {
    TImage &Image = _Target.Images.emplace_back();
    Image.URI = (_ModelDirectory / SourceImage.uri).string();
  }

  for (const tinygltf::Sampler &SourceSampler : _Source.samplers)
  {
    TSampler &Sampler = _Target.Samplers.emplace_back();
    Sampler.WrapS = ToTextureWrapMode(SourceSampler.wrapS);
    Sampler.WrapT = ToTextureWrapMode(SourceSampler.wrapT);
    Sampler.MinFilter = ToTextureFilterMode(SourceSampler.minFilter);
    Sampler.MagFilter = ToTextureFilterMode(SourceSampler.magFilter);
  }
}

void CTinyGLTFParseStrategy::GenerateTangentsIfMissing(TPrimitive &Primitive)
{
  const bool HasTangent = Primitive.Attributes.contains(EAttributeType::Tangent);
  const bool HasPosition = Primitive.Attributes.contains(EAttributeType::Position);
  const bool HasNormal = Primitive.Attributes.contains(EAttributeType::Normal);
  const bool HasTex = Primitive.Attributes.contains(EAttributeType::TexCoords_0);

  if (HasTangent || !HasPosition || !HasNormal || !HasTex || Primitive.Indices.empty())
    return;

  const TAttribute &PosAttr = Primitive.Attributes.at(EAttributeType::Position);
  const TAttribute &NorAttr = Primitive.Attributes.at(EAttributeType::Normal);
  const TAttribute &UvAttr = Primitive.Attributes.at(EAttributeType::TexCoords_0);

  const size_t vertCount = PosAttr.Data.size() / PosAttr.ByteStride;

  std::vector<glm::vec3> positions(vertCount), normals(vertCount);
  std::vector<glm::vec2> uvs(vertCount);

  auto readVec = [](const TAttribute &A, size_t idx, int compCount) -> std::vector<float>
  {
    std::vector<float> out(compCount, 0.0f);
    const size_t stride = static_cast<size_t>(A.ByteStride);
    const uint8_t *base = A.Data.data() + idx * stride;
    for (int c = 0; c < compCount; ++c)
      std::memcpy(&out[c], base + c * sizeof(float), sizeof(float));
    return out;
  };

  for (size_t i = 0; i < vertCount; ++i)
  {
    auto p = readVec(PosAttr, i, 3);
    positions[i] = glm::vec3(p[0], p[1], p[2]);
    auto n = readVec(NorAttr, i, 3);
    normals[i] = glm::vec3(n[0], n[1], n[2]);
    auto t = readVec(UvAttr, i, 2);
    uvs[i] = glm::vec2(t[0], t[1]);
  }

  assert(Primitive.IndicesCount != 0 && "How is it 0?");

  // Determine index size (1, 2, or 4 bytes)
  const std::vector<uint8_t> &indicesRaw = Primitive.Indices;
  const size_t indexCount = Primitive.IndicesCount;
  const size_t indexSize = indicesRaw.size() / indexCount;
  // Defensive: fallback to 1 if indexCount is 0
  if (indexSize != 1 && indexSize != 2 && indexSize != 4)
    return;

  // Convert indices to uint32_t for tangent generation
  std::vector<uint32_t> indices(indexCount);
  for (size_t i = 0; i < indexCount; ++i)
  {
    switch (indexSize)
    {
    case 1:
      indices[i] = indicesRaw[i];
      break;
    case 2:
      indices[i] = *reinterpret_cast<const uint16_t *>(&indicesRaw[i * 2]);
      break;
    case 4:
      indices[i] = *reinterpret_cast<const uint32_t *>(&indicesRaw[i * 4]);
      break;
    }
  }

  struct MikkUserData
  {
    const std::vector<glm::vec3> *pos;
    const std::vector<glm::vec3> *nor;
    const std::vector<glm::vec2> *uv;
    const std::vector<uint32_t> *idx;
    std::vector<glm::vec3> tan1;
    std::vector<glm::vec3> tan2;
    std::vector<glm::vec4> out;
  } data;

  data.pos = &positions;
  data.nor = &normals;
  data.uv = &uvs;
  data.idx = &indices;
  data.tan1.assign(vertCount, glm::vec3(0.0f));
  data.tan2.assign(vertCount, glm::vec3(0.0f));
  data.out.assign(vertCount, glm::vec4(0.0f));

  SMikkTSpaceInterface iface{};
  iface.m_getNumFaces = [](const SMikkTSpaceContext *ctx) -> int
  {
    auto *d = (MikkUserData *)ctx->m_pUserData;
    return static_cast<int>(d->idx->size() / 3);
  };

  iface.m_getNumVerticesOfFace = [](const SMikkTSpaceContext *, const int) -> int
  {
    return 3;
  };

  iface.m_getPosition = [](const SMikkTSpaceContext *ctx, float fvPosOut[], const int iFace, const int iVert)
  {
    auto *d = (MikkUserData *)ctx->m_pUserData;
    uint32_t vi = (*d->idx)[iFace * 3 + iVert];
    const glm::vec3 &p = (*d->pos)[vi];
    fvPosOut[0] = p.x;
    fvPosOut[1] = p.y;
    fvPosOut[2] = p.z;
  };

  iface.m_getNormal = [](const SMikkTSpaceContext *ctx, float fvNormOut[], const int iFace, const int iVert)
  {
    auto *d = (MikkUserData *)ctx->m_pUserData;
    uint32_t vi = (*d->idx)[iFace * 3 + iVert];
    const glm::vec3 &n = (*d->nor)[vi];
    fvNormOut[0] = n.x;
    fvNormOut[1] = n.y;
    fvNormOut[2] = n.z;
  };

  iface.m_getTexCoord = [](const SMikkTSpaceContext *ctx, float fvTexcOut[], const int iFace, const int iVert)
  {
    auto *d = (MikkUserData *)ctx->m_pUserData;
    uint32_t vi = (*d->idx)[iFace * 3 + iVert];
    const glm::vec2 &t = (*d->uv)[vi];
    fvTexcOut[0] = t.x;
    fvTexcOut[1] = t.y;
  };

  iface.m_setTSpace = [](const SMikkTSpaceContext *ctx, const float fvTangent[], const float fvBiTangent[], const float, const float, const tbool, const int iFace, const int iVert)
  {
    auto *d = (MikkUserData *)ctx->m_pUserData;
    uint32_t vi = (*d->idx)[iFace * 3 + iVert];
    d->tan1[vi] += glm::vec3(fvTangent[0], fvTangent[1], fvTangent[2]);
    d->tan2[vi] += glm::vec3(fvBiTangent[0], fvBiTangent[1], fvBiTangent[2]);
  };

  SMikkTSpaceContext ctx{};
  ctx.m_pInterface = &iface;
  ctx.m_pUserData = &data;

  genTangSpaceDefault(&ctx);

  for (size_t v = 0; v < vertCount; ++v)
  {
    const glm::vec3 &N = normals[v];
    glm::vec3 T = data.tan1[v];
    T = glm::normalize(T - N * glm::dot(N, T));
    if (!(std::isfinite(T.x) && std::isfinite(T.y) && std::isfinite(T.z)))
    {
      glm::vec3 up = (std::fabs(N.y) < 0.999f) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
      T = glm::normalize(glm::cross(up, N));
    }
    float w = (glm::dot(glm::cross(N, T), data.tan2[v]) < 0.0f) ? -1.0f : 1.0f;
    data.out[v] = glm::vec4(T, w);
  }

  TAttribute TangentAttr;
  TangentAttr.ComponentType = EAttributeComponentType::Float;
  TangentAttr.Type = 4;
  TangentAttr.ByteStride = static_cast<int>(4 * sizeof(float));
  TangentAttr.Data.resize(vertCount * 4 * sizeof(float));
  std::memcpy(TangentAttr.Data.data(), data.out.data(), TangentAttr.Data.size());

  Primitive.Attributes.emplace(EAttributeType::Tangent, std::move(TangentAttr));
}
