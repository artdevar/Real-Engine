#pragma once

#include "ModelData.h"
#include "interfaces/Asset.h"
#include "interfaces/ModelParseStrategy.h"
#include <memory>
#include <vector>

class CModel : public IAsset
{
  DISABLE_CLASS_COPY(CModel);

public:
  CModel(std::unique_ptr<IModelParseStrategy> _ParseStrategy);

  void Shutdown() override;

  bool Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>) override;

  bool IsLoaded() const;

  const TModelData &GetModelData() const;

protected:
  std::unique_ptr<IModelParseStrategy> m_ParseStrategy;
  std::unique_ptr<TModelData>          m_Model;
};