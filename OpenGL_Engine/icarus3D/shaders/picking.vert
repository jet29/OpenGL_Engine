#version 330 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vPos;

// Uniforms
// -----------------------
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(vPos, 1.0f);
}