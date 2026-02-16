#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aTexCoords_0;

out vec2 io_TexCoords;

uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;

void main()
{
    io_TexCoords = aTexCoords_0;
    gl_Position = u_LightSpaceMatrix * u_Model * vec4(aPos, 1.0);
}