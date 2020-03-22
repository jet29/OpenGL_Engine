#version 430 core
// Vertex color (interpolated/fragment)
in vec3 vColor;
// Vertex texture position (interpolated/fragment)
in vec2 vTexPos;
// texture
uniform sampler2D image;

// Fragment Color
out vec4 color;

void main(){

    vec4 temp = vec4(texture(image,vTexPos));

    if(temp.a < 0.3) discard;

    color = temp;
    //color = vec4(1.0,0.0,0.0,1.0);
}