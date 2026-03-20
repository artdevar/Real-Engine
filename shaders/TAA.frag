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

void main()
{
  vec2 velocity = texture(VelocityTexture, io_TexCoords).xy;

  vec2 jitterDelta = Jitter - PrevJitter;
  vec2 uv = io_TexCoords - velocity * InverseScreenSize + jitterDelta;
  uv = clamp(uv, vec2(0.0), vec2(1.0));

  vec3 current = texture(CurrentFrame, io_TexCoords).rgb;
  vec3 history = texture(HistoryFrame, uv).rgb;

  float currentDepth = texture(DepthTexture, io_TexCoords).r;
  float historyDepth = texture(DepthTexture, uv).r;

  if (abs(currentDepth - historyDepth) > 0.0005)
    history = current;

  vec3 minC = vec3(999);
  vec3 maxC = vec3(-999);

  for (int y = -1; y <= 1; y++)
  {
    for (int x = -1; x <= 1; x++)
    {
      vec2 offset = vec2(x, y) * InverseScreenSize;
      vec3 color  = texture(CurrentFrame, io_TexCoords + offset).rgb;

      minC = min(minC, color);
      maxC = max(maxC, color);
    }
  }

  history = clamp(history, minC, maxC);
  vec3 color = mix(current, history, 0.1);
  o_FragColor = vec4(color, 1.0);
}