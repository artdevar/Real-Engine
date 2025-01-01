#include "ComponentsFactory.h"
#include "tiny_gltf.h"
#include "graphics/Model.h"
#include "graphics/ShaderTypes.h"
#include "graphics/Texture.h"

namespace ecs
{

static void ParseMesh(const tinygltf::Model & _Model, const tinygltf::Mesh & _Mesh, TModelComponent & _Component)
{
  for (const tinygltf::Primitive & Primitive : _Mesh.primitives)
  {
    TModelComponent::TPrimitiveData & PrimitiveData = _Component.Primitives.emplace_back();

    if (auto IterPosition = Primitive.attributes.find("POSITION"); IterPosition != Primitive.attributes.end())
    {
      const tinygltf::Accessor &   Accessor   = _Model.accessors[IterPosition->second];
      const tinygltf::BufferView & BufferView = _Model.bufferViews[Accessor.bufferView];
      const tinygltf::Buffer &     Buffer     = _Model.buffers[BufferView.buffer];

      PrimitiveData.VAO.Bind();

      CVertexBuffer VBO(GL_STATIC_DRAW);
      VBO.Bind();
      VBO.Assign(&Buffer.data[BufferView.byteOffset], BufferView.byteLength);

      PrimitiveData.VAO.EnableAttrib(ATTRIB_LOC_POSITION, Accessor.type, Accessor.componentType, Accessor.ByteStride(BufferView), (void*)(Accessor.byteOffset));
      PrimitiveData.VAO.Unbind();
    }

    if (auto IterNormal = Primitive.attributes.find("NORMAL"); IterNormal != Primitive.attributes.end())
    {
      const tinygltf::Accessor &   Accessor   = _Model.accessors[IterNormal->second];
      const tinygltf::BufferView & BufferView = _Model.bufferViews[Accessor.bufferView];
      const tinygltf::Buffer &     Buffer     = _Model.buffers[BufferView.buffer];

      PrimitiveData.VAO.Bind();

      CVertexBuffer VBO(GL_STATIC_DRAW);
      VBO.Bind();
      VBO.Assign(&Buffer.data[BufferView.byteOffset], BufferView.byteLength);

      PrimitiveData.VAO.EnableAttrib(ATTRIB_LOC_NORMAL, Accessor.type, Accessor.componentType, Accessor.ByteStride(BufferView), (void*)(Accessor.byteOffset));
      PrimitiveData.VAO.Unbind();
    }

    if (Primitive.indices >= 0)
    {
      const tinygltf::Accessor &   Accessor   = _Model.accessors[Primitive.indices];
      const tinygltf::BufferView & BufferView = _Model.bufferViews[Accessor.bufferView];
      const tinygltf::Buffer &     Buffer     = _Model.buffers[BufferView.buffer];

      PrimitiveData.Indices = Accessor.count;
      PrimitiveData.Offset  = Accessor.byteOffset;

      PrimitiveData.VAO.Bind();

      CElementBuffer EBO(GL_STATIC_DRAW);
      EBO.Bind();
      EBO.Assign(&Buffer.data[BufferView.byteOffset], BufferView.byteLength);

      PrimitiveData.VAO.Unbind();
    }
  }
}

static void ParseNodes(const tinygltf::Model & _Model, const tinygltf::Node & _Node, TModelComponent & _Component)
{
  if (_Node.mesh >= 0)
    ParseMesh(_Model, _Model.meshes[_Node.mesh], _Component);

  for (int NodeChild : _Node.children)
  {
    assert(NodeChild >= 0 && NodeChild < _Model.nodes.size());
    ParseNodes(_Model, _Model.nodes[NodeChild], _Component);
  }
}

void CComponentsFactory::CreateModelComponent(const std::shared_ptr<CModel> & _Model, TModelComponent & _Component)
{
  const tinygltf::Model & NativeModel = _Model->m_Model;
  const tinygltf::Scene & Scene       = NativeModel.scenes[NativeModel.defaultScene];

  for (int NodeIndex : Scene.nodes)
  {
    assert(NodeIndex >= 0 && NodeIndex < NativeModel.nodes.size());
    ParseNodes(NativeModel, NativeModel.nodes[NodeIndex], _Component);
  }
}

void CComponentsFactory::CreateSkyboxComponent(const std::shared_ptr<CTextureBase> & _Skybox, TSkyboxComponent & _Component)
{
  _Component.TextureUnit = _Skybox->Get();
}

#if 0

void drawMesh(const std::map<int, GLuint>& vbos,
              tinygltf::Model &model, tinygltf::Mesh &mesh) {
  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    tinygltf::Primitive primitive = mesh.primitives[i];
    tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.at(indexAccessor.bufferView));

    glDrawElements(primitive.mode, indexAccessor.count,
                   indexAccessor.componentType,
                   BUFFER_OFFSET(indexAccessor.byteOffset));
  }
}

// recursively draw node and children nodes of model
void drawModelNodes(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos,
                    tinygltf::Model &model, tinygltf::Node &node) {
  if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
    drawMesh(vaoAndEbos.second, model, model.meshes[node.mesh]);
  }
  for (size_t i = 0; i < node.children.size(); i++) {
    drawModelNodes(vaoAndEbos, model, model.nodes[node.children[i]]);
  }
}
void drawModel(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos,
               tinygltf::Model &model) {
  glBindVertexArray(vaoAndEbos.first);

  const tinygltf::Scene &scene = model.scenes[model.defaultScene];
  for (size_t i = 0; i < scene.nodes.size(); ++i) {
    drawModelNodes(vaoAndEbos, model, model.nodes[scene.nodes[i]]);
  }

  glBindVertexArray(0);
}



void bindMesh(std::map<int, GLuint>& vbos, tinygltf::Model &model, tinygltf::Mesh &mesh)
{
  for (size_t i = 0; i < model.bufferViews.size(); ++i)
  {
    const tinygltf::BufferView &bufferView = model.bufferViews[i];
    if (bufferView.target == 0)
    {
      continue;  // Unsupported bufferView.
                 /*
                   From spec2.0 readme:
                   https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
                            ... drawArrays function should be used with a count equal to
                   the count            property of any of the accessors referenced by the
                   attributes            property            (they are all equal for a given
                   primitive).
                 */
    }

    const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
    std::cout << "bufferview.target " << bufferView.target << std::endl;

    GLuint vbo;
    glGenBuffers(1, &vbo);
    vbos[i] = vbo;
    glBindBuffer(bufferView.target, vbo);

    std::cout << "buffer.data.size = " << buffer.data.size()
              << ", bufferview.byteOffset = " << bufferView.byteOffset
              << std::endl;

    glBufferData(bufferView.target, bufferView.byteLength,
                 &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
  }

  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    tinygltf::Primitive primitive = mesh.primitives[i];
    tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

    for (auto &attrib : primitive.attributes) {
      tinygltf::Accessor accessor = model.accessors[attrib.second];
      int byteStride =
          accessor.ByteStride(model.bufferViews[accessor.bufferView]);
      glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

      int size = 1;
      if (accessor.type != TINYGLTF_TYPE_SCALAR) {
        size = accessor.type;
      }

      int vaa = -1;
      if (attrib.first.compare("POSITION") == 0) vaa = 0;
      if (attrib.first.compare("NORMAL") == 0) vaa = 1;
      if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;

      if (vaa > -1) {
        glEnableVertexAttribArray(vaa);
        glVertexAttribPointer(vaa, size, accessor.componentType,
                              accessor.normalized ? GL_TRUE : GL_FALSE,
                              byteStride, BUFFER_OFFSET(accessor.byteOffset));
      } else
        std::cout << "vaa missing: " << attrib.first << std::endl;
    }

    if (model.textures.size() > 0) {
      // fixme: Use material's baseColor
      tinygltf::Texture &tex = model.textures[0];

      if (tex.source > -1) {

        GLuint texid;
        glGenTextures(1, &texid);

        tinygltf::Image &image = model.images[tex.source];

        glBindTexture(GL_TEXTURE_2D, texid);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        GLenum format = GL_RGBA;

        if (image.component == 1) {
          format = GL_RED;
        } else if (image.component == 2) {
          format = GL_RG;
        } else if (image.component == 3) {
          format = GL_RGB;
        } else {
          // ???
        }

        GLenum type = GL_UNSIGNED_BYTE;
        if (image.bits == 8) {
          // ok
        } else if (image.bits == 16) {
          type = GL_UNSIGNED_SHORT;
        } else {
          // ???
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
                     format, type, &image.image.at(0));
      }
  }
}

void bindModelNodes(std::map<int, GLuint>& vbos, tinygltf::Model &model, tinygltf::Node &node)
{
  if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
    bindMesh(vbos, model, model.meshes[node.mesh]);
  }

  for (size_t i = 0; i < node.children.size(); i++) {
    assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
    bindModelNodes(vbos, model, model.nodes[node.children[i]]);
  }
}

std::pair<GLuint, std::map<int, GLuint>> bindModel(tinygltf::Model &model)
{
  std::map<int, GLuint> vbos;
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  const tinygltf::Scene &scene = model.scenes[model.defaultScene];
  for (size_t i = 0; i < scene.nodes.size(); ++i) {
    assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
    bindModelNodes(vbos, model, model.nodes[scene.nodes[i]]);
  }

  glBindVertexArray(0);
  // cleanup vbos but do not delete index buffers yet
  for (auto it = vbos.cbegin(); it != vbos.cend();) {
    tinygltf::BufferView bufferView = model.bufferViews[it->first];
    if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER) {
      glDeleteBuffers(1, &vbos[it->first]);
      vbos.erase(it++);
    }
    else {
      ++it;
    }
  }

  return {vao, vbos};
}
#endif

}