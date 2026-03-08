#version 460 core

in vec3 io_WorldPos;

out vec4 o_FragColor;

void main()
{
  float dist = length(io_WorldPos.xz);
  float fade = max(0.0, 1.0 - (dist / 50.0));

  o_FragColor = vec4(vec3(0.5), fade * 0.8);
}
