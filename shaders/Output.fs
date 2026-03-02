#version 460 core

in vec2  io_TexCoords;
out vec4 o_FragColor;

uniform sampler2D Texture;

void main()
{
  o_FragColor = texture(Texture, io_TexCoords);
}
