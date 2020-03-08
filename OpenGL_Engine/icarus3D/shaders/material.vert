#version 330 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vertexPosition;
// Attribute 2 of the vertex
layout (location = 1) in vec2 vertexUV;
// Atributte 3 of the normals
layout (location = 2) in vec3 vertexNormal;


out Data{
    vec3 vertexPos;
    vec3 normal;
    vec2 uv;
}dataOut;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    mat4 modelView = view * model;
    mat4 MVP = projection * modelView;
    mat4 normalMatrix = transpose(inverse(modelView));
    // World space vertex
    dataOut.vertexPos = vec3(model*vec4(vertexPosition,1.f));
    // World space normal
    dataOut.normal  = vec3( normalMatrix * vec4(vertexNormal,0.0) );
    // UV coordinates
    dataOut.uv = vertexUV;

    gl_Position = MVP * vec4(vertexPosition, 1.0f);
}