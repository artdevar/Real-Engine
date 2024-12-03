#include "Components.h"
#include "graphics/TextureBase.h"
#include "graphics/Model.h"

namespace ecs
{

// Components
// Skybox

#if 0
TSkyboxComponent::TSkyboxComponent()
{
  VAO.Bind();
  VBO.Bind();
  VBO.BufferVertices(shared::SkyboxVertices, ARRAY_SIZE(shared::SkyboxVertices), GL_STATIC_DRAW);

  VAO.EnableVertexAttrib(0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

  VBO.Unbind();
  VAO.Unbind();
}
#endif

// Model

#if 0

void TModelComponent::SetModel(const std::shared_ptr<TModel> &_Model)
{
  assert(_Model != nullptr);

  Model = _Model;

  for (const TMesh & Mesh : Model->Meshes)
  {
    IndicesAmount.push_back(Mesh.Indices.size());

    CVertexArray vao;
    CVertexBuffer vbo;
    CElementBuffer ebo;

    vao.Bind();

    vbo.Bind();
    vbo.BufferVertices(Mesh.Vertices, GL_STATIC_DRAW);

    ebo.Bind();
    ebo.BufferIndices(Mesh.Indices, GL_STATIC_DRAW);

    vao.EnableVertexAttrib(0, 3, GL_FLOAT, sizeof(shared::TVertex), (void *)offsetof(shared::TVertex, Position));
    vao.EnableVertexAttrib(1, 3, GL_FLOAT, sizeof(shared::TVertex), (void *)offsetof(shared::TVertex, Normal));
    vao.EnableVertexAttrib(2, 2, GL_FLOAT, sizeof(shared::TVertex), (void *)offsetof(shared::TVertex, TexCoords));
    vao.EnableVertexAttrib(3, 3, GL_FLOAT, sizeof(shared::TVertex), (void *)offsetof(shared::TVertex, Tangent));
    vao.EnableVertexAttrib(4, 3, GL_FLOAT, sizeof(shared::TVertex), (void *)offsetof(shared::TVertex, Bitangent));

    vao.Unbind();

    VAO.push_back(std::move(vao));
    VBO.push_back(std::move(vbo));
    EBO.push_back(std::move(ebo));
  }
}

#endif

// Systems
// Model



}