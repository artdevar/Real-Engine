#version 460 core

in vec2  io_TexCoords;
out vec4 o_FragColor;

uniform sampler2D DepthTexture;
uniform sampler2D CurrentFrame;
uniform sampler2D HistoryFrame;
uniform sampler2D VelocityTex;

uniform vec2 Jitter;
uniform vec2 PrevJitter;
uniform vec2 InverseScreenSize;

vec3 SampleHistoryCatmullRom(vec2 uv)
{
  vec2 texel = InverseScreenSize;

  vec3 c0 = texture(HistoryFrame, uv - texel).rgb;
  vec3 c1 = texture(HistoryFrame, uv).rgb;
  vec3 c2 = texture(HistoryFrame, uv + texel).rgb;
  vec3 c3 = texture(HistoryFrame, uv + texel * 2.0).rgb;

  return (-0.5 * c0 + 1.5 * c1 - 1.5 * c2 + 0.5 * c3);
}

void main()
{
  vec3  current = texture(CurrentFrame, io_TexCoords).rgb;
  float depth   = texture(DepthTexture, io_TexCoords).r;
  if (depth >= 1.0)
  {
    o_FragColor = vec4(current, 1.0);
    return;
  }
  vec2 velocity = texture(VelocityTex, io_TexCoords).xy;

  vec2 jitterDelta = (Jitter - PrevJitter) * InverseScreenSize * 0.5;
  vec2 historyUV   = io_TexCoords - velocity + jitterDelta;
  historyUV        = clamp(historyUV, vec2(0.0), vec2(1.0));
  vec3 history     = SampleHistoryCatmullRom(historyUV);

  float motion   = length(velocity);
  float feedback = mix(0.95, 0.6, clamp(motion * 50.0, 0.0, 1.0));

  vec3 minC = vec3(999);
  vec3 maxC = vec3(-999);

  for (int y = -1; y <= 1; y++)
  {
    for (int x = -1; x <= 1; x++)
    {
      vec2 offset = vec2(x, y) * InverseScreenSize;
      vec3 c      = texture(CurrentFrame, io_TexCoords + offset).rgb;

      minC = min(minC, c);
      maxC = max(maxC, c);
    }
  }

  history = clamp(history, minC, maxC);

  vec3 color  = mix(current, history, feedback);
  o_FragColor = vec4(color, 1.0);
}