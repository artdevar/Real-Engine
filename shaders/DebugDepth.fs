#version 460 core

in vec2 io_TexCoords;
out vec4 o_FragColor;

uniform sampler2D u_DepthMap;
uniform float u_NearPlane;
uniform float u_FarPlane;

// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * u_NearPlane * u_FarPlane) / (u_FarPlane + u_NearPlane - z * (u_FarPlane - u_NearPlane));
}

void main()
{
    float depthValue = texture(u_DepthMap, io_TexCoords).r;
    // o_FragColor = vec4(vec3(LinearizeDepth(depthValue) / u_FarPlane), 1.0); // perspective
    o_FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}