#pragma once

#include "Shared.h"
#include "Texture.h"
#include "interfaces/Asset.h"
#include "utils/Common.h"
#include "tiny_gltf.h"
#include <vector>
#include <memory>

struct aiNode;
struct aiMesh;
struct aiScene;

struct TMaterial
{
  DISABLE_CLASS_COPY(TMaterial);

  TMaterial() = default;

  TMaterial(TMaterial && _Other) :
    DiffuseTexture(std::move(_Other.DiffuseTexture)),
    SpecularTexture(std::move(_Other.SpecularTexture)),
    Shininess(_Other.Shininess)
  {}

  std::shared_ptr<CTextureBase> DiffuseTexture;
  std::shared_ptr<CTextureBase> SpecularTexture;
  float Shininess = 1.0f;
};

struct TMesh
{
  DISABLE_CLASS_COPY(TMesh);

  TMesh(
      std::vector<shared::TVertex> && _Vertices,
      std::vector<unsigned int> && _Indices,
      TMaterial && _Material
    ) :
    Vertices(std::move(_Vertices)),
    Indices(std::move(_Indices)),
    Material(std::move(_Material))
  {}

  TMesh(TMesh && _Other) :
    Vertices(std::move(_Other.Vertices)),
    Indices(std::move(_Other.Indices)),
    Material(std::move(_Other.Material))
  {}

  std::vector<shared::TVertex> Vertices;
  std::vector<unsigned int>    Indices;
  TMaterial                    Material;
};

#if 0
class CModel : public IAsset
{
  DISABLE_CLASS_COPY(CModel);

public:

  CModel() = default;

  void Shutdown() override;

  bool Load(const std::filesystem::path & _Path) override;

  const std::vector<TMesh> & GetMeshes() const;

private:

  struct TLoadingParams
  {
    const aiScene *       Scene;
    std::filesystem::path ModelPath;
  };

  static void ProcessNode(TLoadingParams & _Params, aiNode * _Node, std::vector<TMesh> & _Meshes);

  static TMesh ProcessMesh(TLoadingParams & _Params, aiMesh * _Mesh);

private:

  std::vector<TMesh> m_Meshes;
};

#endif



class CModel : public IAsset
{
  DISABLE_CLASS_COPY(CModel);

public:

  CModel() = default;

  void Shutdown() override;

  bool Load(const std::filesystem::path & _Path) override;

  tinygltf::Model m_Model;
};