#version 460 core

in vec2  io_TexCoords;
out vec4 o_FragColor;

uniform sampler2D CurrentFrame;
uniform sampler2D HistoryFrame;
uniform sampler2D VelocityTexture;
uniform sampler2D DepthTexture;

uniform vec2 Jitter;
uniform vec2 PrevJitter;
uniform vec2 InverseScreenSize;

vec3 CatmullRom(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t)
{
  float t2 = t * t;
  float t3 = t2 * t;

  return 0.5 * (
    2.0 * p1 +
    (-p0 + p2) * t +
    (2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3) * t2 +
    (-p0 + 3.0 * p1 - 3.0 * p2 + p3) * t3
  );
}

vec3 SampleCatmullRom(sampler2D tex, vec2 uv)
{
  vec2 texSize = 1.0 / InverseScreenSize;
  vec2 texCoord = uv * texSize;
  vec2 fractCoord = fract(texCoord);

  vec2 centerCoord = (floor(texCoord) + 0.5) * InverseScreenSize;

  vec3 c00 = texture(tex, centerCoord + vec2(-1.0, -1.0) * InverseScreenSize).rgb;
  vec3 c10 = texture(tex, centerCoord + vec2( 0.0, -1.0) * InverseScreenSize).rgb;
  vec3 c20 = texture(tex, centerCoord + vec2( 1.0, -1.0) * InverseScreenSize).rgb;
  vec3 c30 = texture(tex, centerCoord + vec2( 2.0, -1.0) * InverseScreenSize).rgb;

  vec3 c01 = texture(tex, centerCoord + vec2(-1.0,  0.0) * InverseScreenSize).rgb;
  vec3 c11 = texture(tex, centerCoord + vec2( 0.0,  0.0) * InverseScreenSize).rgb;
  vec3 c21 = texture(tex, centerCoord + vec2( 1.0,  0.0) * InverseScreenSize).rgb;
  vec3 c31 = texture(tex, centerCoord + vec2( 2.0,  0.0) * InverseScreenSize).rgb;

  vec3 c02 = texture(tex, centerCoord + vec2(-1.0,  1.0) * InverseScreenSize).rgb;
  vec3 c12 = texture(tex, centerCoord + vec2( 0.0,  1.0) * InverseScreenSize).rgb;
  vec3 c22 = texture(tex, centerCoord + vec2( 1.0,  1.0) * InverseScreenSize).rgb;
  vec3 c32 = texture(tex, centerCoord + vec2( 2.0,  1.0) * InverseScreenSize).rgb;

  vec3 c03 = texture(tex, centerCoord + vec2(-1.0,  2.0) * InverseScreenSize).rgb;
  vec3 c13 = texture(tex, centerCoord + vec2( 0.0,  2.0) * InverseScreenSize).rgb;
  vec3 c23 = texture(tex, centerCoord + vec2( 1.0,  2.0) * InverseScreenSize).rgb;
  vec3 c33 = texture(tex, centerCoord + vec2( 2.0,  2.0) * InverseScreenSize).rgb;

  vec3 x0 = CatmullRom(c00, c10, c20, c30, fractCoord.x);
  vec3 x1 = CatmullRom(c01, c11, c21, c31, fractCoord.x);
  vec3 x2 = CatmullRom(c02, c12, c22, c32, fractCoord.x);
  vec3 x3 = CatmullRom(c03, c13, c23, c33, fractCoord.x);

  return CatmullRom(x0, x1, x2, x3, fractCoord.y);
}

void main()
{
  vec2 velocity = texture(VelocityTexture, io_TexCoords).xy;

  vec2 jitterDelta = Jitter - PrevJitter;
  vec2 uv = io_TexCoords - velocity * InverseScreenSize + jitterDelta;
  uv = clamp(uv, vec2(0.0), vec2(1.0));

  vec3 current = texture(CurrentFrame, io_TexCoords).rgb;
  vec3 history = SampleCatmullRom(HistoryFrame, uv);

  float currentDepth = texture(DepthTexture, io_TexCoords).r;
  float historyDepth = texture(DepthTexture, uv).r;

  if (abs(currentDepth - historyDepth) > 0.0005)
    history = current;

  vec3 minC = vec3(999);
  vec3 maxC = vec3(-999);
  vec3 avgC = vec3(0.0);

  for (int y = -1; y <= 1; y++)
  {
    for (int x = -1; x <= 1; x++)
    {
      vec2 offset = vec2(x, y) * InverseScreenSize;
      vec3 color  = texture(CurrentFrame, io_TexCoords + offset).rgb;

      minC = min(minC, color);
      maxC = max(maxC, color);
      avgC += color;
    }
  }

  avgC /= 9.0;

  vec3 deviation = maxC - minC;
  minC = avgC - deviation * 0.5;
  maxC = avgC + deviation * 0.5;

  history = clamp(history, minC, maxC);

  float blendFactor = 0.1;
  vec3 color = mix(current, history, blendFactor);

  o_FragColor = vec4(color, 1.0);
}