#version 330 core

// Ouput data
out vec4 fragColor;

// Values that stay constant for the whole mesh.
uniform vec3 pickingColor;

void main(){
	fragColor = vec4(pickingColor,1.0f);
}