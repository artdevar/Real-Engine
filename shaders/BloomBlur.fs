#version 460 core

in vec2  io_TexCoords;
out vec4 o_FragColor;

uniform sampler2D Image;
uniform bool      Horizontal;
uniform float     Weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
  vec2 tex_offset = 1.0 / textureSize(Image, 0);
  vec3 result     = texture(Image, io_TexCoords).rgb * Weights[0];

  if (Horizontal)
  {
    for (int i = 1; i < 5; ++i)
    {
      result += texture(Image, io_TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * Weights[i];
      result += texture(Image, io_TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * Weights[i];
    }
  }
  else
  {
    for (int i = 1; i < 5; ++i)
    {
      result += texture(Image, io_TexCoords + vec2(0.0, tex_offset.y * i)).rgb * Weights[i];
      result += texture(Image, io_TexCoords - vec2(0.0, tex_offset.y * i)).rgb * Weights[i];
    }
  }

  o_FragColor = vec4(result, 1.0);
}
