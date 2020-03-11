#version 330 core
out vec4 fragColor;


uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform bool lightSwitch;

in Data{
    vec3 fragPos;
    vec3 normal;
    vec2 uv;
}dataIn;

uniform sampler2D albedo; // Diffuse map

void main()
{
    vec3 lightSwitch = vec3(lightSwitch);
    fragColor=vec4((ambient*0.3 + specular*0.3 + diffuse) * lightSwitch, 1.0f);
}
