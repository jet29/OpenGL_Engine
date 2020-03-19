#version 330 core
// Fragment Color
out vec4 fragColor;

// Vertex uv (interpolated/fragment)
in vec2 TexCoords;

uniform sampler2D ssao1;
uniform sampler2D ssao2;
uniform mat4 projection;


void main()
{
	vec3 vPos = vec3(projection * vec4(texture(ssao1,TexCoords).rgb,1.0f));
	fragColor = vec4(texture(ssao1,TexCoords).rgb,1.0f);
}