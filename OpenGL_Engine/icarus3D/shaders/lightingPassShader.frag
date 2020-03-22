#version 330 core

#define MAX_LIGHTS 200

// Vertex uv (interpolated/fragment)
in vec2 TexCoords;

// Textures with model data
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssao;


// Light color contribution properties
struct LightColor{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Model's materials
struct Material{
    vec3 ka;
    vec3 kd;
    vec3 ks;
    float shininess;
};

// Point light attenuation properties
struct Attenuation{
    float constant;
    float linear;
    float quadratic;
};

// Directional light 
struct DirectionalLight {
   vec3 direction;
   LightColor color;
   bool lightSwitch;
};

// Point light
struct PointLight{
    vec3 position;
    LightColor color;
    Attenuation attenuation;
	bool lightSwitch;
};

uniform Material material;
uniform DirectionalLight dirlight;
uniform int numOfPointLight;
uniform PointLight pointlight[MAX_LIGHTS];
uniform vec3 viewPos;

// Light casters functions
vec3 calcDirLight(DirectionalLight light,vec3 normal, vec3 viewDir, vec3 albedo, float ambientOcclusion);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 albedo, vec3 fragPos, float ambientOcclusion);
// Illumination  Techniques functions
float calcDiffLighting(vec3 normal,vec3 lightDir);
float calcSpecLighting(vec3 normal,vec3 lightDir, vec3 viewDir);


// Fragment Color
out vec4 fragColor;

void main(){
    // Retrieve data from G-buffer
    vec3 fragPos = texture(gPosition,TexCoords).xyz;
    vec3 normal = texture(gNormal,TexCoords).xyz;
    // Discard fragment if normal = 0
    if(normal == vec3(0.0f)) discard;
    // Model color with textures included
    vec3 albedo = texture(gAlbedoSpec, TexCoords).xyz;
//    vec3 normal = normalize(dataIn.normal);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 result = vec3(0.0f);
    float ambientOcclusion = texture(ssao, TexCoords).r;
    // Directional color contribution
    result += calcDirLight(dirlight, normal, viewDir, albedo, ambientOcclusion) * vec3(dirlight.lightSwitch);
    // Pointlights color contribution
    for(int i=0;i<numOfPointLight;i++){
        result += calcPointLight(pointlight[i], normal, viewDir, albedo, fragPos, ambientOcclusion) * vec3(pointlight[i].lightSwitch);
    }
    fragColor=vec4(result, 1.0f);
}

vec3 calcDirLight(DirectionalLight light,vec3 normal, vec3 viewDir, vec3 albedo, float ambientOcclusion){
    // Getting light direction vector (Only direction since this is a directional only light)
    vec3 lightDir = normalize(-light.direction);
	// Diffuse lighting technique
    float diff = calcDiffLighting(normal,lightDir);
	// Specular lighting technique
    float spec = calcSpecLighting(normal,lightDir,viewDir);
    // Ambient lighting (only 20% of contribution in final fragment color)
	vec3 ambient  = light.color.ambient  * ambientOcclusion;
	// Diffuse lighting
	vec3 diffuse  = light.color.diffuse * diff  ; 
    // Specular lighting
	vec3 specular = light.color.specular  * spec ;
	// Return value
	return (ambient + diffuse + specular) * albedo;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 albedo, vec3 fragPos, float ambientOcclusion){
    // Compute light direction vector
    vec3 lightDir = normalize(light.position - fragPos);
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.attenuation.constant + light.attenuation.linear * distance + light.attenuation.quadratic * distance * distance);
    
   // Diffuse lighting technique
    float diff = calcDiffLighting(normal,lightDir);
    // Specular lighting technique 
    float spec = calcSpecLighting(normal,lightDir,viewDir);
    // Ambient color
    vec3 ambient = light.color.ambient * attenuation * ambientOcclusion;
    // Diffuse color
    vec3 diffuse = light.color.diffuse * diff * attenuation;
    // Specular color
    vec3 specular = light.color.specular * spec * attenuation;

    return (ambient + diffuse + specular) * albedo;
}

float calcDiffLighting(vec3 normal,vec3 lightDir){
	float NdotL = clamp(dot(normal,lightDir),0.0f,1.0f);
    // Blinn-Phong Diffuse contribution
	return NdotL;
}

float calcSpecLighting(vec3 normal,vec3 lightDir,vec3 viewDir){
    vec3 reflectDir = reflect(-lightDir, normal);
	// Blinn-Phong Specular contribution
	return clamp(dot(viewDir,reflectDir),0.0f,1.0f);
}