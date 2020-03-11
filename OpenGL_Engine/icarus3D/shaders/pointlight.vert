#version 330 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vertexPosition;
// Attribute 2 of the vertex
layout (location = 1) in vec2 vertexUV;
// Atributte 1 of the vertex
layout (location = 2) in vec3 vertexNormal;

out Data{
    vec3 fragPos;
    vec3 normal;
    vec2 uv;
}dataOut;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main(){
    dataOut.uv = vertexUV;
    dataOut.normal = mat3(transpose(inverse(model))) * vertexNormal;
    dataOut.fragPos = vec3(model*vec4(vertexPosition, 1.0f));
    gl_Position = projection * view * vec4(dataOut.fragPos, 1.0f);

}