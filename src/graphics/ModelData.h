#pragma once

#include <vector>
#include <map>
#include <cstdint>

enum class EAttributeType
{
  Position,
  Normal,
  TexCoords
};

struct TNode
{
  std::vector<int> Children;
  int Mesh = -1;
};

struct TAttribute
{
  std::vector<uint8_t> Data;
  int Size;
  int Type;
  int ByteStride;
  int ByteOffset;
};

struct TPrimitive
{
  std::map<EAttributeType, TAttribute> Attributes;
  std::vector<uint8_t>                 Indices;

  int count, offset;
};

struct TMesh
{
  std::vector<TPrimitive> Primitives;
};

struct TScene
{
  std::vector<int> Nodes;
};

struct TAccessor
{

};

struct TModelData
{
  std::vector<TScene>    Scenes;
  std::vector<TNode>     Nodes;
  std::vector<TMesh>     Meshes;
  std::vector<TAccessor> Accessors;
};