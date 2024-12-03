#pragma once

#include "Shared.h"
#include "Buffer.h"
#include "TextureBase.h"
#include "interfaces/Renderable.h"
#include "utils/Common.h"
#include <vector>
#include <memory>

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

struct TModel
{
  DISABLE_CLASS_COPY(TModel);

  TModel(std::vector<TMesh> && _Meshes) :
    Meshes(std::move(_Meshes))
  {}

  std::vector<TMesh> Meshes;
};
