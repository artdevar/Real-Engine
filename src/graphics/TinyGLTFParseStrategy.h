#pragma once

#include "interfaces/ModelParseStrategy.h"
#include "tiny_gltf.h"

class CTinyGLTFParseStrategy : public IModelParseStrategy
{
public:
  bool Parse(const std::filesystem::path &_Path, TModelData &_Model) override;

private:
  void ParseModel(const tinygltf::Model &_Source, TModelData &_Target, const std::filesystem::path &_ModelDirectory);
  void ParseScene(const tinygltf::Model &_Source, TModelData &_Target);
  void ParseNodes(const tinygltf::Model &_Source, TModelData &_Target);
  void ParseMeshes(const tinygltf::Model &_Source, TModelData &_Target);
  void ParseAttributes(const tinygltf::Model &_Source, const tinygltf::Primitive &_SourcePrimitive, TPrimitive &_TargetPrimitive);
  void ParseIndices(const tinygltf::Model &_Source, const tinygltf::Primitive &_SourcePrimitive, TPrimitive &_TargetPrimitive);
  void ParseMaterials(const tinygltf::Model &_Source, TModelData &_Target);
  void ParseImages(const tinygltf::Model &_Source, TModelData &_Target, const std::filesystem::path &_ModelDirectory);
};