#version 460 core

struct TMaterial
{
  sampler2D BaseColorTexture;

  float AlphaCutoff;
  int   AlphaMode;
};

uniform TMaterial u_Material;

in vec2 io_TexCoords;

void main()
{
    bool isEnableCutoff = u_Material.AlphaMode == 1 || u_Material.AlphaMode == 2; // Treat BLEND as MASK in the depth pass

    vec4 baseColorSample = texture(u_Material.BaseColorTexture, io_TexCoords);
    if (isEnableCutoff && baseColorSample.a < u_Material.AlphaCutoff)
        discard;

    // gl_FragDepth = gl_FragCoord.z;
}