#version 460 core

out vec4 o_FragColor;

in vec3 io_TexCoords;

uniform samplerCube u_Cubemap;

void main()
{
  o_FragColor = texture(u_Cubemap, io_TexCoords);
}