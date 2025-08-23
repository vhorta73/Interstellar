#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform vec2  u_Offset; // movement offset from CPU
uniform float u_Zoom;   // zoom scale factor

out vec2 v_TexCoord;

void main() {
    // Scale the position around the origin (centered)
    vec2 zoomedPosition = a_Position.xy * u_Zoom;

    // Apply movement offset after scaling
    gl_Position = vec4(zoomedPosition + u_Offset, a_Position.z, 1.0);

    // Pass through texture coordinates
    v_TexCoord = a_TexCoord;
}
