#include "Model.h"
#include "utils/Logger.h"

CModel::CModel(std::unique_ptr<IModelParseStrategy> && _ParseStrategy) :
  m_ParseStrategy(std::move(_ParseStrategy))
{
}

void CModel::Shutdown()
{
  m_ParseStrategy.reset();
  m_Model.reset();
}

bool CModel::Load(const std::filesystem::path & _Path)
{
  m_Model = std::make_unique<TModelData>();

  const bool IsParsed = m_ParseStrategy->Parse(_Path, *m_Model);
  if (!IsParsed)
    m_Model.reset();

  return IsParsed;
}

bool CModel::IsLoaded() const
{
  return m_Model != nullptr;
}

const TModelData & CModel::GetModelData() const
{
  assert(IsLoaded());
  return *m_Model;
}

#if 0
void CModel::Shutdown()
{
  m_Meshes.clear();
}

bool CModel::Load(const std::filesystem::path & _Path)
{
  std::optional<std::filesystem::path> ModelPath;
  bool FlipUVs = false;

  if (IsJson(_Path))
  {
    nlohmann::json JsonContent = utils::ParseJson(_Path);
    ModelPath = _Path.parent_path() / JsonContent["ModelPath"].get<std::string>();
    FlipUVs   = JsonContent["FlipUVs"].get<bool>();
  }

  unsigned int Flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices;
  if (FlipUVs)
    Flags |= aiProcess_FlipUVs;

  const aiScene *  Scene;
  Assimp::Importer Importer;

  Scene = Importer.ReadFile(ModelPath.value_or(_Path).string(), Flags);
  if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
  {
    CLogger::Log(ELogType::Error, std::format("Assimp import model '{}' error: {}\n", ModelPath->c_str(), Importer.GetErrorString()));
    return false;
  }

  TLoadingParams Params;
  Params.ModelPath = ModelPath.value_or(_Path);
  Params.Scene     = Scene;

  ProcessNode(Params, Scene->mRootNode, m_Meshes);

  CLogger::Log(ELogType::Info, std::format("Model '{}' is loaded. Meshes count: {}\n", Params.ModelPath.c_str(), m_Meshes.size()));

  return true;
}

const std::vector<TMesh> & CModel::GetMeshes() const
{
  return m_Meshes;
}

void CModel::ProcessNode(TLoadingParams &_Params, aiNode *_Node, std::vector<TMesh> & _Meshes)
{
  for (unsigned int i = 0; i < _Node->mNumMeshes; i++)
  {
    aiMesh * Mesh = _Params.Scene->mMeshes[_Node->mMeshes[i]];
    _Meshes.push_back(ProcessMesh(_Params, Mesh));
  }

  for (unsigned int i = 0; i < _Node->mNumChildren; i++)
    ProcessNode(_Params, _Node->mChildren[i], _Meshes);
}

TMesh CModel::ProcessMesh(TLoadingParams & _Params, aiMesh * _Mesh)
{
  assert(_Mesh->HasNormals());

  std::vector<shared::TVertex> Vertices;
  std::vector<unsigned int>    Indices;
  TMaterial                    Material;

  for (unsigned int i = 0; i < _Mesh->mNumVertices; i++)
  {
    shared::TVertex & Vertex = Vertices.emplace_back();

    Vertex.Position = glm::vec3(_Mesh->mVertices[i].x, _Mesh->mVertices[i].y, _Mesh->mVertices[i].z);
    Vertex.Normal   = glm::vec3(_Mesh->mNormals[i].x, _Mesh->mNormals[i].y, _Mesh->mNormals[i].z);

    if (!_Mesh->mTextureCoords[0])
    {
      Vertex.TexCoords = glm::vec2(0.0f, 0.0f);
      continue;
    }

    Vertex.TexCoords = glm::vec2(_Mesh->mTextureCoords[0][i].x, _Mesh->mTextureCoords[0][i].y);
    Vertex.Tangent   = glm::vec3(_Mesh->mTangents[i].x, _Mesh->mTangents[i].y, _Mesh->mTangents[i].z);
    Vertex.Bitangent = glm::vec3(_Mesh->mBitangents[i].x, _Mesh->mBitangents[i].y, _Mesh->mBitangents[i].z);
  }

  for (unsigned int i = 0; i < _Mesh->mNumFaces; i++)
  {
    aiFace Face = _Mesh->mFaces[i];
    for (unsigned int j = 0; j < Face.mNumIndices; j++)
      Indices.push_back(Face.mIndices[j]);
  }

  aiMaterial * AiMaterial = _Params.Scene->mMaterials[_Mesh->mMaterialIndex];

  auto GetMaterialTexture = [AiMaterial, &_Params, ResourceManager = CEngine::Instance()->GetResourceManager()](aiTextureType _Type)
  {
    aiString Path;
    if (AiMaterial->GetTexture(_Type, 0, &Path) == AI_SUCCESS)
      return ResourceManager->LoadTexture(_Params.ModelPath.parent_path() / Path.C_Str());
    else
      return std::shared_ptr<CTextureBase>(nullptr);
  };

  Material.DiffuseTexture  = GetMaterialTexture(aiTextureType::aiTextureType_DIFFUSE);
  Material.SpecularTexture = GetMaterialTexture(aiTextureType::aiTextureType_SPECULAR);
  Material.Shininess       = 32.0f;

  return TMesh(std::move(Vertices), std::move(Indices), std::move(Material));
}

#endif