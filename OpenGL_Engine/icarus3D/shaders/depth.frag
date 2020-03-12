#version 330 core
out vec4 fragColor;

struct LightColor{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


struct DirectionalLightProperties {
   vec3 direction;
   LightColor color;
};

in Data{
    vec3 vertexPos;
    vec3 normal;
}dataIn;


uniform DirectionalLightProperties light;

uniform vec3 viewPos;
uniform float shininess;

void main()
{
    //fragColor=vec4(lightContribution, 1.f);
}