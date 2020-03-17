#version 330 core
// MRT
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

struct Material{
    vec3 ka;
    vec3 kd;
    vec3 ks;
    float shininess;
};

in Data{
    vec3 fragPos;
    vec3 normal;
    vec2 uv;
}dataIn;

uniform Material material;
uniform vec3 viewPos;
uniform sampler2D albedo;

void main(){
    // Store fragment position vector in first gBuffer texture
    gPosition = dataIn.fragPos;
    // Store fragment normal vector in second gBuffer texture
    gNormal = normalize(dataIn.normal);
    // Store fragment MTL color in third gBuffer texture
    gAlbedoSpec = texture(albedo,dataIn.uv);
}