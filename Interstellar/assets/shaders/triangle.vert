#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec2 i_Offset;   // per-instance world offset

uniform vec2  u_Offset;  // camera offset in NDC space (typically -center * zoom)
uniform float u_Zoom;    // camera zoom (NDC per world unit)

out vec2 v_TexCoord;

void main() {
    // world -> clip: clip.xy = (world * u_Zoom) + u_Offset
    vec2 world = a_Position.xy + i_Offset;
    vec2 clipXY = world * u_Zoom + u_Offset;

    gl_Position = vec4(clipXY, a_Position.z, 1.0);
    v_TexCoord = a_TexCoord;
}
