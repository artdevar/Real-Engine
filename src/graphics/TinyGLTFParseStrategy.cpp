#include "TinyGLTFParseStrategy.h"
#include "utils/Logger.h"

#include "utils/Stopwatch.h"

bool CTinyGLTFParseStrategy::Parse(const std::filesystem::path & _Path, TModelData & _Model)
{
  tinygltf::Model    NativeModel;
  tinygltf::TinyGLTF Loader;
  std::string        Error, Warning;

  const bool IsLoaded = Loader.LoadASCIIFromFile(&NativeModel, &Error, &Warning, _Path.string());

  if (!Warning.empty())
    CLogger::Log(ELogType::Warning, "Model loading {} warning {}\n", _Path.c_str(), Warning);

  if (!Error.empty())
    CLogger::Log(ELogType::Error, "Model loading {} error {}\n", _Path.c_str(), Error);

  if (!IsLoaded)
  {
    CLogger::Log(ELogType::Error, "Model loading {} failed\n", _Path.c_str());
    return false;
  }

  CLogger::Log(ELogType::Info, "Model {} is loaded\n", _Path.c_str());

  CStopwatch s("TinyGltf Parse");
  ParseModel(NativeModel, _Model);

  return true;
}

void CTinyGLTFParseStrategy::ParseModel(const tinygltf::Model & _ParseModel, TModelData & _Model)
{
  const tinygltf::Scene & ParseScene = _ParseModel.scenes[_ParseModel.defaultScene];

  TScene & Scene = _Model.Scenes.emplace_back();
  Scene.Nodes.reserve(ParseScene.nodes.size());
  std::copy(ParseScene.nodes.begin(), ParseScene.nodes.end(), std::back_inserter(Scene.Nodes));

  for (const tinygltf::Node & ParseNode : _ParseModel.nodes)
  {
    TNode & Node = _Model.Nodes.emplace_back();
    Node.Mesh = ParseNode.mesh;
    Node.Children.reserve(ParseNode.children.size());
    std::copy(ParseNode.children.begin(), ParseNode.children.end(), std::back_inserter(Node.Children));
  }

  for (const tinygltf::Mesh & ParseMesh : _ParseModel.meshes)
  {
    TMesh & Mesh = _Model.Meshes.emplace_back();

    for (const tinygltf::Primitive & ParsePrimitive : ParseMesh.primitives)
    {
      TPrimitive & Primitive = Mesh.Primitives.emplace_back();

      if (auto IterPosition = ParsePrimitive.attributes.find("POSITION"); IterPosition != ParsePrimitive.attributes.end())
      {
        const tinygltf::Accessor &   Accessor   = _ParseModel.accessors[IterPosition->second];
        const tinygltf::BufferView & BufferView = _ParseModel.bufferViews[Accessor.bufferView];
        const tinygltf::Buffer &     Buffer     = _ParseModel.buffers[BufferView.buffer];

        TAttribute Attribute;
        Attribute.ByteOffset = Accessor.byteOffset;
        Attribute.ByteStride = Accessor.ByteStride(BufferView);
        Attribute.Size       = Accessor.type;
        Attribute.Type       = Accessor.componentType;

        const size_t ActualOffset = BufferView.byteOffset;
        const size_t SizeInBytes  = BufferView.byteLength;

        Attribute.Data.reserve(SizeInBytes);
        std::copy(Buffer.data.begin() + ActualOffset, Buffer.data.begin() + ActualOffset + SizeInBytes, std::back_inserter(Attribute.Data));

        Primitive.Attributes.emplace(EAttributeType::Position, std::move(Attribute));
      }

      if (auto IterNormal = ParsePrimitive.attributes.find("NORMAL"); IterNormal != ParsePrimitive.attributes.end())
      {
        const tinygltf::Accessor &   Accessor   = _ParseModel.accessors[IterNormal->second];
        const tinygltf::BufferView & BufferView = _ParseModel.bufferViews[Accessor.bufferView];
        const tinygltf::Buffer &     Buffer     = _ParseModel.buffers[BufferView.buffer];

        TAttribute Attribute;
        Attribute.ByteOffset = Accessor.byteOffset;
        Attribute.ByteStride = Accessor.ByteStride(BufferView);
        Attribute.Size       = Accessor.type;
        Attribute.Type       = Accessor.componentType;

        const size_t ActualOffset = BufferView.byteOffset;
        const size_t SizeInBytes  = BufferView.byteLength;

        Attribute.Data.reserve(SizeInBytes);
        std::copy(Buffer.data.begin() + ActualOffset, Buffer.data.begin() + ActualOffset + SizeInBytes, std::back_inserter(Attribute.Data));

        Primitive.Attributes.emplace(EAttributeType::Normal, std::move(Attribute));
      }

      if (ParsePrimitive.indices >= 0)
      {
        const tinygltf::Accessor &   Accessor   = _ParseModel.accessors[ParsePrimitive.indices];
        const tinygltf::BufferView & BufferView = _ParseModel.bufferViews[Accessor.bufferView];
        const tinygltf::Buffer &     Buffer     = _ParseModel.buffers[BufferView.buffer];

        Primitive.count  = Accessor.count;
        Primitive.offset = Accessor.byteOffset;

        Primitive.Indices.reserve(BufferView.byteLength);
        std::copy(Buffer.data.begin() + BufferView.byteOffset, Buffer.data.begin() + BufferView.byteOffset + BufferView.byteLength, std::back_inserter(Primitive.Indices));
      }
    }
  }
}


