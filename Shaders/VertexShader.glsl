#version 450
// Color Vertex Shader
layout(std140, binding = 1) uniform CameraConstants {
    mat4 viewProj;
    mat4 modelViewProj;
    mat4 model;
};
layout(location = 0) in vec4 a_Positions;
layout(location = 0) out flat uvec2 o_TexCoord;
void main() {
    gl_Position = modelViewProj * a_Positions;
    int face = gl_VertexIndex / 6;
    o_TexCoord = uvec2(face, 0);
}