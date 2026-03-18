#include "Model.h"
#include <common/Logger.h>
#include "utils/Path.h"

CModel::CModel(std::unique_ptr<IModelParseStrategy> _ParseStrategy) :
    m_ParseStrategy(std::move(_ParseStrategy))
{
}

void CModel::Shutdown()
{
  m_ParseStrategy.reset();
  m_Model.reset();
}

bool CModel::Load(const std::filesystem::path &_Path, CPasskey<CResourceManager>)
{
  LOG_INFO("[CModel] Loading model from '{}'", utils::GetRelativePath(_Path).string());

  m_Model = std::make_unique<TModelData>();

  const bool IsParsed = m_ParseStrategy->Parse(_Path, *m_Model);
  if (IsParsed)
    LOG_INFO("[CModel] Model '{}' loaded successfully", utils::GetRelativePath(_Path).string());
  else
    m_Model.reset();

  return IsParsed;
}

bool CModel::IsLoaded() const
{
  return m_Model != nullptr;
}

const TModelData &CModel::GetModelData() const
{
  assert(IsLoaded());
  return *m_Model;
}
