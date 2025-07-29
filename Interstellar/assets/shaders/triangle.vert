#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform vec2 u_Offset; // movement offset from CPU

out vec2 v_TexCoord;

void main() {
    gl_Position = vec4(a_Position.xy + u_Offset, a_Position.z, 1.0);
    v_TexCoord = a_TexCoord;
}
