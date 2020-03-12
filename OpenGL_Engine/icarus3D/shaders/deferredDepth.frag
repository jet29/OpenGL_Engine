#version 330 core
// Vertex color (interpolated/fragment)
in vec3 vColor;
// Vertex texture position (interpolated/fragment)
in vec2 vTexPos;
// texture
uniform sampler2D depthMap;
uniform sampler2D image;
uniform isampler1D kernel7;
uniform isampler1D kernel11;

uniform float near_plane;
uniform float far_plane;

uniform float DOFThreshold = 0.5;

// Fragment Color
out vec4 color;

// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

void main(){

    vec3 finalColor;

    float depthValue = texture(depthMap, vTexPos).r;
    vec3 colorValue = texture(image,vTexPos).rgb;

    float linearizedValue = LinearizeDepth(depthValue) / (far_plane);

    vec3 texColor  = texture(image,vTexPos).xyz;
	ivec2 texSize  = textureSize(image, 0);
	ivec2 texIndex = ivec2(vTexPos.xy * vec2(texSize.xy));
	vec3 mean      = vec3(0.0f);

    if(linearizedValue > 0.50){

	    ivec2 pivot = ivec2(11/2,11/2);
        // Kernel application
        for (int i=0 ;i<11;i++){
            for (int j=0 ; j<11;j++){
                mean += texelFetch(image, texIndex + ivec2(i-pivot.x, j-pivot.y), 0).xyz * texelFetch(kernel11, i * 11 + j,0).r;
            }
        }
        mean /= 121;

        color = vec4(mean,1.0);
    }
    else if(linearizedValue > 0.30f && linearizedValue < 0.50f){

        ivec2 pivot = ivec2(7/2,7/2);

        // Kernel application
        for (int i=0 ;i<7;i++){
            for (int j=0 ; j<7;j++){
                mean += texelFetch(image, texIndex + ivec2(i-pivot.x, j-pivot.y), 0).xyz * texelFetch(kernel11, i * 7 + j,0).r;
            }
        }
        mean /= 49;

        color = vec4(mean,1.0);
    }
    else if(linearizedValue < 0.30){
        color =  vec4(texColor,1.0);
    }

}