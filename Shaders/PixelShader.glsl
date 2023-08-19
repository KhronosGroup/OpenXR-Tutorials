#version 450
// Texture Fragment Shader
layout(location = 0) in flat uvec2 i_TexCoord;
layout(location = 1) in  vec3 i_Normal;
layout(location = 0) out vec4 o_Color;
layout(std140, binding = 2) uniform Data {
    vec4 colors[6];
}
d_Data;
void main() {
    uint i = i_TexCoord.x;
	float light=0.2+0.8*clamp(i_Normal.g,0.0,1.0);
    o_Color = vec4(light*d_Data.colors[0].rgb,1.0);
}