#version 460 core

in vec2  io_TexCoords;
out vec4 o_FragColor;

uniform sampler2D Texture;
uniform vec2      InverseScreenSize;
uniform bool      IsFXAAEnabled;
uniform bool      IsHDR;
uniform float     Exposure;
uniform bool      IsGammaCorrectionEnabled;
uniform float     Gamma;

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

  float lumaRange = lumaMax - lumaMin;
  if (lumaRange < 0.031)
    return rgbM;

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

vec3 ToneMapACES(vec3 x)
{
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;

  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main()
{
  vec3 texel = texture(Texture, io_TexCoords).rgb;

  vec3 color;
  if (IsFXAAEnabled)
    color = CalculateFXAA(texel);
  else
    color = texel;

  if (IsHDR)
  {
    color *= Exposure;
    color  = ToneMapACES(color);
  }

  if (IsGammaCorrectionEnabled)
    color.rgb = pow(color.rgb, vec3(1.0 / Gamma));

  o_FragColor = vec4(color, 1.0);
}