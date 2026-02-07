#include "TinyGLTFParseStrategy.h"
#include "utils/Logger.h"
#include "utils/Stopwatch.h"
#include <glm/gtc/type_ptr.hpp>

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

    if (!SourceNode.translation.empty())
    {
      Node.Translation = glm::vec3(
          SourceNode.translation[0],
          SourceNode.translation[1],
          SourceNode.translation[2]);
    }

    if (!SourceNode.rotation.empty())
    {
      Node.Rotation = glm::quat(
          static_cast<float>(SourceNode.rotation[3]),
          static_cast<float>(SourceNode.rotation[0]),
          static_cast<float>(SourceNode.rotation[1]),
          static_cast<float>(SourceNode.rotation[2]));
    }

    if (!SourceNode.scale.empty())
    {
      Node.Scale = glm::vec3(
          SourceNode.scale[0],
          SourceNode.scale[1],
          SourceNode.scale[2]);
    }
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
      Primitive.MaterialIndex = SourcePrimitive.material;

      ParseAttributes(_Source, SourcePrimitive, Primitive);
      ParseIndices(_Source, SourcePrimitive, Primitive);
    }
  }
}

void CTinyGLTFParseStrategy::ParseAttributes(const tinygltf::Model &_Source, const tinygltf::Primitive &_SourcePrimitive, TPrimitive &_TargetPrimitive)
{
  for (const auto &[Name, AccessorIndex] : _SourcePrimitive.attributes)
  {
    const EAttributeType Type = Name == "POSITION" ? EAttributeType::Position : Name == "NORMAL"   ? EAttributeType::Normal
                                                                            : Name == "TEXCOORD_0" ? EAttributeType::TexCoords
                                                                            : Name == "TANGENT"    ? EAttributeType::Tangent
                                                                                                   : static_cast<EAttributeType>(-1);

    if (Type == static_cast<EAttributeType>(-1))
      continue;

    const tinygltf::Accessor &Accessor = _Source.accessors[AccessorIndex];
    const tinygltf::BufferView &BufferView = _Source.bufferViews[Accessor.bufferView];
    const tinygltf::Buffer &Buffer = _Source.buffers[BufferView.buffer];

    TAttribute Attribute;
    Attribute.ComponentType = Accessor.componentType;
    Attribute.ByteStride = Accessor.ByteStride(BufferView);
    Attribute.Type = Accessor.type;

    const size_t Offset = BufferView.byteOffset + Accessor.byteOffset;
    const size_t ComponentSize = tinygltf::GetComponentSizeInBytes(Accessor.componentType);
    const size_t ComponentCount = tinygltf::GetNumComponentsInType(Accessor.type);
    const size_t ElementSize = ComponentSize * ComponentCount;
    const size_t Stride = Attribute.ByteStride == 0 ? ElementSize : static_cast<size_t>(Attribute.ByteStride);
    const size_t Size = static_cast<size_t>(Accessor.count) * Stride;

    if (Offset + Size > Buffer.data.size())
    {
      CLogger::Log(ELogType::Error, "Attribute data out of bounds (name: {}, offset: {}, size: {}, buffer: {})", Name, Offset, Size, Buffer.data.size());
      continue;
    }

    Attribute.Data.reserve(Size);
    Attribute.Data.insert(Attribute.Data.end(), Buffer.data.begin() + Offset, Buffer.data.begin() + Offset + Size);

    _TargetPrimitive.Attributes.emplace(Type, std::move(Attribute));
  }
}

void CTinyGLTFParseStrategy::ParseIndices(const tinygltf::Model &_Source, const tinygltf::Primitive &_SourcePrimitive, TPrimitive &_TargetPrimitive)
{
  if (_SourcePrimitive.indices < 0)
    return;

  const tinygltf::Accessor &Accessor = _Source.accessors[_SourcePrimitive.indices];
  const tinygltf::BufferView &BufferView = _Source.bufferViews[Accessor.bufferView];
  const tinygltf::Buffer &Buffer = _Source.buffers[BufferView.buffer];

  _TargetPrimitive.IndicesCount = Accessor.count;

  const size_t Offset = BufferView.byteOffset + Accessor.byteOffset;
  const size_t ComponentSize = tinygltf::GetComponentSizeInBytes(Accessor.componentType);
  const size_t Size = static_cast<size_t>(Accessor.count) * ComponentSize;

  if (Offset + Size > Buffer.data.size())
  {
    CLogger::Log(ELogType::Error, "Index data out of bounds (offset: {}, size: {}, buffer: {})", Offset, Size, Buffer.data.size());
    return;
  }

  _TargetPrimitive.Indices.reserve(Size);
  _TargetPrimitive.Indices.insert(_TargetPrimitive.Indices.end(), Buffer.data.begin() + Offset, Buffer.data.begin() + Offset + Size);
}

void CTinyGLTFParseStrategy::ParseMaterials(const tinygltf::Model &_Source, TModelData &_Target)
{
  _Target.Materials.reserve(_Source.materials.size());

  for (const tinygltf::Material &SourceMaterial : _Source.materials)
  {
    TMaterial &Material = _Target.Materials.emplace_back();
    const tinygltf::PbrMetallicRoughness &PBR = SourceMaterial.pbrMetallicRoughness;

    if (PBR.baseColorTexture.index >= 0)
      Material.BaseColorTextureIndex = PBR.baseColorTexture.index;

    if (PBR.metallicRoughnessTexture.index >= 0)
      Material.MetallicRoughnessTextureIndex = PBR.metallicRoughnessTexture.index;

    if (SourceMaterial.normalTexture.index >= 0)
      Material.NormalTextureIndex = SourceMaterial.normalTexture.index;

    const std::vector<double> &Factor = PBR.baseColorFactor;
    Material.BaseColorFactor = glm::vec4(Factor[0], Factor[1], Factor[2], Factor[3]);
    Material.MetallicFactor = static_cast<float>(PBR.metallicFactor);
    Material.RoughnessFactor = static_cast<float>(PBR.roughnessFactor);
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
}
