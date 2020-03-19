#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

in Data{
    vec3 fragPos;
    vec3 normal;
    vec2 uv;
}dataIn;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = dataIn.fragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(dataIn.normal);
    // and the diffuse per-fragment color
    gAlbedo.rgb = vec3(0.95);
}