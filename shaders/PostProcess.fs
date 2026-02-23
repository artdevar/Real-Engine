#version 460 core

in vec2  io_TexCoords;
out vec4 o_FragColor;

uniform sampler2D Texture;
uniform vec2      InverseScreenSize;
uniform bool      IsFXAAEnabled;
uniform bool      IsHDR;
uniform float     Exposure;

float Luma(vec3 color)
{
  return dot(color, vec3(0.299, 0.587, 0.114));
}

vec3 CalculateFXAA(vec3 color)
{
  vec2 texel = InverseScreenSize;

  vec3 rgbNW = texture(Texture, io_TexCoords + vec2(-texel.x, -texel.y)).rgb;
  vec3 rgbNE = texture(Texture, io_TexCoords + vec2(texel.x, -texel.y)).rgb;
  vec3 rgbSW = texture(Texture, io_TexCoords + vec2(-texel.x, texel.y)).rgb;
  vec3 rgbSE = texture(Texture, io_TexCoords + vec2(texel.x, texel.y)).rgb;
  vec3 rgbM  = color;

  float lumaNW = Luma(rgbNW);
  float lumaNE = Luma(rgbNE);
  float lumaSW = Luma(rgbSW);
  float lumaSE = Luma(rgbSE);
  float lumaM  = Luma(rgbM);

  float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
  float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

  vec2 dir;
  dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
  dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

  float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * 0.0312, 0.0078125);
  float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);

  dir = clamp(dir * rcpDirMin, vec2(-8.0), vec2(8.0)) * texel;

  vec3 rgbA = 0.5 * (texture(Texture, io_TexCoords + dir * (1.0 / 3.0 - 0.5)).rgb + texture(Texture, io_TexCoords + dir * (2.0 / 3.0 - 0.5)).rgb);
  vec3 rgbB = rgbA * 0.5 + 0.25 * (texture(Texture, io_TexCoords + dir * -0.5).rgb + texture(Texture, io_TexCoords + dir * 0.5).rgb);

  float lumaB = Luma(rgbB);

  if ((lumaB < lumaMin) || (lumaB > lumaMax))
    return rgbA;
  else
    return rgbB;
}

void main()
{
  vec3 texel = texture(Texture, io_TexCoords).rgb;

  vec4 color;
  if (IsFXAAEnabled)
    color = vec4(CalculateFXAA(texel), 1.0);
  else
    color = vec4(texel, 1.0);

  if (IsHDR)
    color = vec4(vec3(1.0) - exp(-color.rgb * Exposure), 1.0);

  o_FragColor = color;
}