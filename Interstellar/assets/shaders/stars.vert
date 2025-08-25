#version 330 core
layout(location = 0) in vec2 a_StarPos;   // world position in “clip-like” units

uniform vec2  u_Offset;  // camera offset (same names as your triangle shaders)
uniform float u_Zoom;    // camera zoom

void main() {
    vec2 p = a_StarPos * u_Zoom + u_Offset;
    gl_Position = vec4(p, 0.0, 1.0);

    // Point size grows with zoom but is clamped for sanity
    float sz = clamp(u_Zoom * 2.0, 1.0, 12.0);
    gl_PointSize = sz;
}
