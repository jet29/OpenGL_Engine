#version 330 core
layout (location = 0) in vec3 vertexPos;
layout (location = 2) in vec2 vertexUV;

out vec2 TexCoords;

void main()
{
    TexCoords = vertexUV;
    gl_Position = vec4(vertexPos, 1.0);
}