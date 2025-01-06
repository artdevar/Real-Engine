#pragma once

#include "interfaces/ModelParseStrategy.h"
#include "tiny_gltf.h"

class CTinyGLTFParseStrategy : public IModelParseStrategy
{
public:

  bool Parse(const std::filesystem::path & _Path, TModelData & _Model) override;

protected:

  void ParseModel(const tinygltf::Model & _ParseModel, TModelData & _Model);

};