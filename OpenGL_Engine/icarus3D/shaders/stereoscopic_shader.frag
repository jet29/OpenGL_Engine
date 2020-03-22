#version 330 core
// Fragment Color
out vec4 fragColor;

// Vertex uv (interpolated/fragment)
in vec2 TexCoords;

uniform sampler2D eyeTexture;
uniform vec3 colorFilter;


vec3 colorize(vec3 targetTexture,vec3 color){
	// turn color into grayscale
	float grayScale = dot(targetTexture, vec3(0.2989,0.5870,0.1140));
	// Now colorize frag color
	return (vec3(grayScale) * color);
}

void main()
{
	vec3 targetEye = colorize(texture(eyeTexture,TexCoords).rgb,colorFilter);
	if(targetEye == vec3(1.0f,0.0f,0.0f)) discard;
	fragColor = vec4(targetEye,1.0f);
}