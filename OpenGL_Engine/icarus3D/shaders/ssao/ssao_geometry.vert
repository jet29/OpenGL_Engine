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

uniform bool invertedNormals;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main(){
    // Compute fragPos in view space
    vec4 viewPos = view * model * vec4(vertexPosition,1.0f);
    dataOut.fragPos = viewPos.xyz;
    // Send texture uv to fragment shader
    dataOut.uv = vertexUV;
    // Compute normals
    mat3 normalMatrix = transpose(inverse(mat3(view*model)));
    dataOut.normal = normalMatrix * -vertexNormal;
    gl_Position = projection * viewPos;
}
    
