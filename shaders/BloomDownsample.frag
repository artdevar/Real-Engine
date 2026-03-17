#version 460 core

in vec2  io_TexCoords;
out vec4 o_FragColor;

uniform sampler2D ColorTexture;
uniform float     Threshold;

void main()
{
  vec3 color = texture(ColorTexture, io_TexCoords).rgb;

  float brightness = max(max(color.r, color.g), color.b);
  float knee       = 0.5;

  float contribution = max(0.0, brightness - Threshold);
  contribution      /= max(0.0001, brightness + knee);

  vec3 bloom = color * contribution;

  o_FragColor = vec4(bloom, 1.0);
}
