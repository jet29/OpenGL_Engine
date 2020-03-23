#version 330 core
out vec4 fragColor;

#define MAX_LIGHTS 200

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

// Point light properties
struct Attenuation{
    float constant;
    float linear;
    float quadratic;
};

// Directional light properties
struct DirectionalLight {
   vec3 direction;
   LightColor color;
   bool lightSwitch;
};

struct PointLight{
    vec3 position;
    LightColor color;
    Attenuation attenuation;
	bool lightSwitch;
};

in Data{
    vec3 fragPos;
    vec3 normal;
    vec2 uv;
}dataIn;

uniform Material material;
uniform DirectionalLight dirlight;
uniform int numOfPointLight;
uniform PointLight pointlight[MAX_LIGHTS];
uniform vec3 viewPos;
uniform sampler2D albedo; // Diffuse map
uniform vec4 colorFilter;
uniform bool anaglyph = false;

// Light casters functions
vec3 calcDirLight(DirectionalLight light,vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir);
// Illumination  Techniques functions
float calcDiffLighting(vec3 normal,vec3 lightDir);
float calcSpecLighting(vec3 normal,vec3 lightDir, vec3 viewDir);

vec4 colorize(vec3 fragColor,vec4 filterColor){
	// turn color into grayscale
	float grayScale = dot(fragColor, vec3(0.2989,0.5870,0.1140));
	// Now colorize frag color
	return vec4(vec3(grayScale) * filterColor.rgb,filterColor.a);
}


void main()
{
    vec3 normal = normalize(dataIn.normal);
    vec3 viewDir = normalize(viewPos - dataIn.fragPos);
    vec3 result = vec3(0.0f,0.0f,0.0f);
    // Directional color contribution
    result += calcDirLight(dirlight, normal, viewDir) * vec3(dirlight.lightSwitch);
    // Pointlights color contribution
    for(int i=0;i<numOfPointLight;i++){
        result += calcPointLight(pointlight[i], normal, viewDir) * vec3(pointlight[i].lightSwitch);
    }
    
    //anaglyph == true ? fragColor=vec4(vec3(result.r*colorFilter.r,result.g*colorFilter.g,result.b*colorFilter.b),colorFilter.a) : fragColor=vec4(result, 1.0f) ;
    //anaglyph == true ? fragColor= colorize(result,colorFilter) : fragColor=vec4(result, 1.0f) ;
    fragColor = vec4(result,1.0f);
    // fragColor = vec4(vec3(colorFilter),0.5f);
    //fragColor=vec4(vec3(result.r,0.0f,0.0f),colorFilter.a);
}

vec3 calcDirLight(DirectionalLight light,vec3 normal, vec3 viewDir){
    // Getting light direction vector (Only direction since this is a directional only light)
    vec3 lightDir = normalize(-light.direction);
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

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir){
    // Compute light direction vector
    vec3 lightDir = normalize(light.position - dataIn.fragPos);
    // Attenuation
    float distance = length(light.position - dataIn.fragPos);
    float attenuation = 1.0f / (light.attenuation.constant + light.attenuation.linear * distance + light.attenuation.quadratic * distance * distance);
    
    // Diffuse lighting technique
    float diff = calcDiffLighting(normal,lightDir);
    // Specular lighting technique 
    float spec = calcSpecLighting(normal,lightDir,viewDir);
    // Ambient color
    vec3 ambient = material.ka * light.color.ambient * attenuation;
    // Diffuse color
    vec3 diffuse = material.kd * light.color.diffuse * diff * attenuation;
    // Specular color
    vec3 specular = material.ks * light.color.specular * spec * attenuation;

    return (ambient + diffuse + specular) * texture(albedo,dataIn.uv).rgb;
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