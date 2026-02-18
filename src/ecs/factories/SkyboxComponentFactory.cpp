#include "../ComponentsFactory.h"
#include "render/Buffer.h"
#include "assets/Texture.h"
#include "utils/Common.h"

namespace ecs
{
static constexpr inline float SKYBOX_VERTICES[] = {
    -1.0f, 1.0f,  -1.0f, //
    -1.0f, -1.0f, -1.0f, //
    1.0f,  -1.0f, -1.0f, //
    1.0f,  -1.0f, -1.0f, //
    1.0f,  1.0f,  -1.0f, //
    -1.0f, 1.0f,  -1.0f, //

    -1.0f, -1.0f, 1.0f,  //
    -1.0f, -1.0f, -1.0f, //
    -1.0f, 1.0f,  -1.0f, //
    -1.0f, 1.0f,  -1.0f, //
    -1.0f, 1.0f,  1.0f,  //
    -1.0f, -1.0f, 1.0f,  //

    1.0f,  -1.0f, -1.0f, //
    1.0f,  -1.0f, 1.0f,  //
    1.0f,  1.0f,  1.0f,  //
    1.0f,  1.0f,  1.0f,  //
    1.0f,  1.0f,  -1.0f, //
    1.0f,  -1.0f, -1.0f, //

    -1.0f, -1.0f, 1.0f, //
    -1.0f, 1.0f,  1.0f, //
    1.0f,  1.0f,  1.0f, //
    1.0f,  1.0f,  1.0f, //
    1.0f,  -1.0f, 1.0f, //
    -1.0f, -1.0f, 1.0f, //

    -1.0f, 1.0f,  -1.0f, //
    1.0f,  1.0f,  -1.0f, //
    1.0f,  1.0f,  1.0f,  //
    1.0f,  1.0f,  1.0f,  //
    -1.0f, 1.0f,  1.0f,  //
    -1.0f, 1.0f,  -1.0f, //

    -1.0f, -1.0f, -1.0f, //
    -1.0f, -1.0f, 1.0f,  //
    1.0f,  -1.0f, -1.0f, //
    1.0f,  -1.0f, -1.0f, //
    -1.0f, -1.0f, 1.0f,  //
    1.0f,  -1.0f, 1.0f   //
};

void CComponentsFactory::CreateSkyboxComponent(const std::shared_ptr<CTextureBase> &_Skybox, TSkyboxComponent &_Component)
{
  _Component.SkyboxTexture = _Skybox->Get();
  _Component.VerticesCount = ARRAY_SIZE(SKYBOX_VERTICES) / 3;

  _Component.VAO.Bind();

  _Component.VBO.Bind();
  _Component.VBO.Assign(SKYBOX_VERTICES, sizeof(SKYBOX_VERTICES));

  _Component.VAO.EnableAttrib(ATTRIB_LOC_POSITION, 3, GL_FLOAT, false, 3 * sizeof(float));
  _Component.VAO.Unbind();
}
} // namespace ecs