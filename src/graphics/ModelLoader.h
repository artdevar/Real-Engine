#pragma once

#include <assimp/Importer.hpp>
#include <vector>
#include <memory>
#include <filesystem>

#include "Shared.h" //

struct aiScene;
struct aiMesh;
struct aiNode;
struct TMesh;
struct TModel;

struct TLoadParams
{
  std::filesystem::path ModelPath;
  bool FlipUVs;
};

class CModelLoader
{
public:

  static std::shared_ptr<TModel> LoadModel(const TLoadParams & _LoadParams);

private:

  struct TProcessParams
  {
    const aiScene * Scene;
    std::filesystem::path ModelPath;
  };

  static void processNode(aiNode* node, const aiScene* scene, std::vector<shared::TVertex>& vertices, std::vector<unsigned int>& indices);

  static void ProcessNode(TProcessParams & _Params, aiNode * _Node, std::vector<TMesh> & _Meshes);

  static TMesh ProcessMesh(TProcessParams & _Params, aiMesh * _Mesh);
};