#include "ModelLoader.h"
#include "Model.h"
#include "TextureBase.h"
#include "Shared.h"
#include "utils/Logger.h"
#include "engine/Engine.h"
#include "engine/ResourceManager.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

std::shared_ptr<TModel> CModelLoader::LoadModel(const TLoadParams & _LoadParams)
{
  const aiScene * Scene;
  Assimp::Importer Importer;

  const std::string ModelPath = _LoadParams.ModelPath.string();

  unsigned int Flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices;
  if (_LoadParams.FlipUVs)
    Flags |= aiProcess_FlipUVs;

  Scene = Importer.ReadFile(ModelPath, Flags);
  if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
  {
    CLogger::Log(ELogType::Error, std::format("Assimp import model '{}' error: {}\n", ModelPath, Importer.GetErrorString()));
    return nullptr;
  }

  TProcessParams Params;
  Params.ModelPath   = _LoadParams.ModelPath;
  Params.Scene       = Scene;

  std::vector<TMesh> Meshes;
  ProcessNode(Params, Scene->mRootNode, Meshes);

  CLogger::Log(ELogType::Info, std::format("Model '{}' is loaded. Meshes count: {}\n", ModelPath, Meshes.size()));

  return std::make_shared<TModel>(std::move(Meshes));
}

void CModelLoader::ProcessNode(TProcessParams &_Params, aiNode *_Node, std::vector<TMesh> &_Meshes)
{
  for (unsigned int i = 0; i < _Node->mNumMeshes; i++)
  {
    aiMesh * Mesh = _Params.Scene->mMeshes[_Node->mMeshes[i]];
    _Meshes.push_back(ProcessMesh(_Params, Mesh));
  }

  for (unsigned int i = 0; i < _Node->mNumChildren; i++)
    ProcessNode(_Params, _Node->mChildren[i], _Meshes);
}

TMesh CModelLoader::ProcessMesh(TProcessParams & _Params, aiMesh * _Mesh)
{
  assert(_Mesh->HasNormals());

  std::vector<shared::TVertex> Vertices;
  std::vector<unsigned int>    Indices;
  TMaterial                    Material;

  for (unsigned int i = 0; i < _Mesh->mNumVertices; i++)
  {
    auto & Vertex = Vertices.emplace_back();

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

  std::shared_ptr<CResourceManager> ResourceManager = CEngine::Instance()->GetResourceManager();

  aiMaterial * AiMaterial = _Params.Scene->mMaterials[_Mesh->mMaterialIndex];

  auto GetMaterialTexture = [AiMaterial, &ResourceManager, &_Params](aiTextureType _Type, bool _LoadDefault) -> std::shared_ptr<CTextureBase> {
    aiString Path;
    if (AiMaterial->GetTexture(_Type, 0, &Path) == AI_SUCCESS)
    {
      return ResourceManager->LoadTexture(_Params.ModelPath.parent_path() / Path.C_Str());
    } else if (_LoadDefault)
    {
      return ResourceManager->LoadTexture("../assets/textures/default.jpg");
    }

    return nullptr;
  };

  Material.DiffuseTexture  = GetMaterialTexture(aiTextureType::aiTextureType_DIFFUSE,  true);
  Material.SpecularTexture = GetMaterialTexture(aiTextureType::aiTextureType_SPECULAR, false);
  Material.Shininess       = 32.0f;

  return TMesh(std::move(Vertices), std::move(Indices), std::move(Material));
}
