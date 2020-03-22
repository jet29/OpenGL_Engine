#version 330 core
// Fragment Color
out vec4 fragColor;

// Vertex uv (interpolated/fragment)
in vec2 TexCoords;

uniform sampler2D texImage;

void main()
{
	fragColor = vec4(vec3(texture(texImage,TexCoords).rgb),1.0f);
}