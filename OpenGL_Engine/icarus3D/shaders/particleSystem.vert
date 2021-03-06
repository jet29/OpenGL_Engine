#version 430 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vertexPosition;
// Atributte 1 of the vertex
layout (location = 1) in vec3 vertexColor;
// Attribute 2 of the vertex
layout (location = 2) in vec2 vertexTexPos;

// Uniforms
//uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;

uniform vec3 particleCenter_worldspace;
uniform vec2 BillboardSize;

// Vertex data out data
out vec3 vColor;
// Vertex texture position
out vec2 vTexPos;

void main()
{
    vec3 vertexPosition_worldspace =
        particleCenter_worldspace
        + CameraRight_worldspace * vertexPosition.x * BillboardSize.x
        + CameraUp_worldspace * vertexPosition.y * BillboardSize.y;

	vTexPos = vertexTexPos;
    vColor = vertexColor;   
    gl_Position = projection * view /* * model*/ * vec4(vertexPosition_worldspace, 1.0f);
}