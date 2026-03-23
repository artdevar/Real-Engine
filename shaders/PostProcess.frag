#version 460 core

in vec2  io_TexCoords;
out vec4 o_FragColor;

uniform sampler2D ColorTexture;
uniform sampler2D DepthTexture;
uniform sampler2D BloomTexture;
uniform sampler2D TAATexture;
uniform vec2      InverseScreenSize;
uniform bool      IsFXAAEnabled;
uniform bool      IsTAAEnabled;
uniform bool      IsHDR;
uniform float     Exposure;
uniform bool      IsBloomEnabled;
uniform float     BloomIntensity;
uniform bool      IsGammaCorrectionEnabled;
uniform float     Gamma;
uniform bool      IsChromaAberrationEnabled;
uniform vec3      ChromaAberrationOffset;

float Luma(vec3 color)
{
  return dot(color, vec3(0.299, 0.587, 0.114));
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

float EdgeFade(vec2 uv)
{
    vec2 d = min(uv, 1.0 - uv);
    float minDist = min(d.x, d.y);
    return smoothstep(0.0, 0.02, minDist);
}

vec3 SampleBase(vec2 uv)
{
  return IsTAAEnabled ? texture(TAATexture, uv).rgb
                      : texture(ColorTexture, uv).rgb;
}

vec3 SampleColor(vec2 uv)
{
  if (!IsChromaAberrationEnabled)
    return SampleBase(uv);

  vec2 center = vec2(0.5);
  vec2 dir = uv - center;

  float dist = length(dir);
  if (dist > 0.0)
    dir /= dist;

  float fade = EdgeFade(uv);
  vec2 baseOffset = dir * dist * fade;

  float r = SampleBase(uv + baseOffset * ChromaAberrationOffset.r).r;
  float g = SampleBase(uv + ChromaAberrationOffset.g).g;
  float b = SampleBase(uv - baseOffset * ChromaAberrationOffset.b).b;

  return vec3(r, g, b);
}

vec3 CalculateFXAA(vec3 color)
{
  vec2 texel = InverseScreenSize;

  vec3 rgbNW = SampleColor(io_TexCoords + vec2(-texel.x, -texel.y));
  vec3 rgbNE = SampleColor(io_TexCoords + vec2(texel.x, -texel.y));
  vec3 rgbSW = SampleColor(io_TexCoords + vec2(-texel.x, texel.y));
  vec3 rgbSE = SampleColor(io_TexCoords + vec2(texel.x, texel.y));
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

  vec3 rgbA = 0.5 * (SampleColor(io_TexCoords + dir * (1.0 / 3.0 - 0.5)) + SampleColor(io_TexCoords + dir * (2.0 / 3.0 - 0.5)));
  vec3 rgbB = rgbA * 0.5 + 0.25 * (SampleColor(io_TexCoords + dir * -0.5) + SampleColor(io_TexCoords + dir * 0.5));

  float lumaB = Luma(rgbB);

  if ((lumaB < lumaMin) || (lumaB > lumaMax))
    return rgbA;
  else
    return rgbB;
}

void main()
{
  vec3 color = SampleColor(io_TexCoords);

  if (IsFXAAEnabled)
  {
    float depth = texture(DepthTexture, io_TexCoords).r;
    if (depth < 1.0)
      color = CalculateFXAA(color);
  }

  if (IsBloomEnabled)
  {
    vec3 bloom = texture(BloomTexture, io_TexCoords).rgb;
    color     += bloom * BloomIntensity;
  }

  if (IsHDR)
  {
    color *= Exposure;
    color  = ToneMapACES(color);
  }

  if (IsGammaCorrectionEnabled)
    color.rgb = pow(color.rgb, vec3(1.0 / Gamma));

  o_FragColor = vec4(color, 1.0);
}