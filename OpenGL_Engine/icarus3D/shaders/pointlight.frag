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

struct DirectionalLight {
   vec3 direction;
   LightColor color;
};

in Data{
    vec3 fragPos;
    vec3 normal;
    vec2 uv;
}dataIn;

uniform Material material;
uniform DirectionalLight light;
uniform vec3 viewPos;
uniform sampler2D albedo; // Diffuse map

vec3 calcDirLight(DirectionalLight light,vec3 normal, vec3 viewDir);
// Illumination  Techniques functions
float calcDiffLighting(vec3 normal,vec3 lightDir);
float calcSpecLighting(vec3 normal,vec3 lightDir, vec3 viewDir);

void main()
{
    vec3 normal = normalize(dataIn.normal);
    vec3 viewDir = normalize(viewPos - dataIn.fragPos);
    vec3 result = vec3(0.0f,0.0f,0.0f);
    // Directional Lighting
    result += calcDirLight(light, normal, viewDir);

    fragColor=vec4(result, 1.0f);
}

vec3 calcDirLight(DirectionalLight light,vec3 normal, vec3 viewDir){
    // Getting light direction vector (Only direction since this is a directional only light)
    vec3 lightDir = normalize(-light.direction);
	//vec3 specular;
	// Diffuse lighting technique
    float diff = calcDiffLighting(normal,lightDir);
	// Specular lighting technique
    float spec = calcSpecLighting(normal,lightDir,viewDir);

	// Ambient lighting (only 20% of contribution in final fragment color)
	vec3 ambient  = light.color.ambient *  material.ka * 0.2;
	// Diffuse lighting
	vec3 diffuse  = light.color.diffuse * diff * material.kd ; 
    // Specular lighting
	vec3 specular = light.color.specular * material.ks * spec ;
	// Return value
	return  (ambient + diffuse + specular) * texture(albedo,dataIn.uv).rgb;
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