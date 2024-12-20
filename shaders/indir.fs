#version 450 core
#extension GL_ARB_bindless_texture : require

layout (location = 0) flat in uint aoDrawId;
layout (location = 1) in vec2      aoTexCoords;

layout(binding = 0, std430) readonly buffer TextureHandles {
  sampler2D bindlessTextures[];
};

out vec4 o_FragColor;

void main()
{
  o_FragColor = vec4(texture(bindlessTextures[aoDrawId], aoTexCoords).rgb, 1.0);
}
