#version 330 core
out vec4 fragColor;

struct LightColor{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material{
    vec3 ka;
    vec3 kd;
    vec3 ks;
    float shininess;
};


struct DirectionalLightProperties {
   vec3 direction;
   LightColor color;
};

in Data{
    vec3 vertexPos;
    vec3 normal;
    vec2 uv;
}dataIn;


uniform Material material;

uniform DirectionalLightProperties light;

uniform vec3 viewPos;

uniform sampler2D albedo; //albedo

void main()
{
    vec3 normal=normalize(dataIn.normal);
    
    vec3 lightContribution = light.color.ambient * material.ka * 0;

    // Direction to the light (Directional Light)
    vec3 lightDir=normalize(-light.direction);

    // Lambert cos(angle(Normal, Light))
    float diff=max(dot(normal,lightDir),0.f);
    vec3 diffuse=diff * light.color.diffuse /* * material.kd */ * texture(albedo,dataIn.uv).rgb;
    lightContribution += diffuse;

    if(diff>0.f){
        // Vector from the vertex to the camera
        vec3 viewDir=normalize(viewPos-dataIn.vertexPos);
        // Blinn-Phong
        vec3 halfwayDir=normalize(lightDir+viewDir);
        float spec=pow(max(dot(normal,halfwayDir),0.f),material.shininess);
        vec3 specular=light.color.specular*spec /* * material.ks */;
        lightContribution += specular;
    }

    fragColor = vec4(lightContribution,1.0);
}