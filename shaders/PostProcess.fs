#version 460 core

in vec2  io_TexCoords;
out vec4 o_FragColor;

uniform sampler2D Texture;

void main()
{
  vec3 color  = texture(Texture, io_TexCoords).rgb;
  o_FragColor = vec4(color, 1.0);
}