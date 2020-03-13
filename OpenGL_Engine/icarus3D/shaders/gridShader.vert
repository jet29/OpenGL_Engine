#version 330 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vertexPosition;

out vec3 vPos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main()
{
    mat4 MVP = projection * view * model;
    vPos = vertexPosition;
    gl_Position = MVP * vec4(vertexPosition, 1.0f);
}