#version 330 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vertexPosition;

//uniform mat4 MVP;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec3 fragPos = vec3 (model * vec4(vertexPosition,1.0f));
    gl_Position = projection * view * vec4(fragPos,1.0f);
}
