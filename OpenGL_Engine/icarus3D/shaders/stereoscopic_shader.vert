#version 330 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vertexPosition;
// Atributte 1 of the vertex
layout (location = 1) in vec3 vertexColor;
// Attribute 2 of the vertex
layout (location = 2) in vec2 vertexTexPos;

// Vertex texture position
out vec2 TexCoords;

void main()
{
	TexCoords = vertexTexPos;
    gl_Position = vec4(vertexPosition, 1.0f);
}