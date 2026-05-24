#version 330 core

layout(location = 0) in vec3 a_Pos;

uniform mat4  u_VP;
uniform vec3  u_CamPos;
uniform float u_PointScale;
uniform float u_MinPx, u_MaxPx;
uniform float u_BrightNear, u_BrightFar;

uniform int   u_Background;     // 0 = near pass, 1 = background pass
uniform float u_BGFixedSizePx;

out float vBright;

void main() {
    vec4 clip = u_VP * vec4(a_Pos, 1.0);
    gl_Position = clip;

    // Keep the background (galactic sky) always visible.
    if (u_Background == 1) {
        vBright = 1.0;
        gl_PointSize = clamp(u_BGFixedSizePx, u_MinPx, u_MaxPx);
        return;
    }

    // Near-field brightness by distance
    float d = distance(u_CamPos, a_Pos);
    vBright = 1.0 - smoothstep(u_BrightNear, u_BrightFar, d);

    float sizePx = (u_PointScale / max(1e-6, gl_Position.w));
    gl_PointSize = clamp(sizePx, u_MinPx, u_MaxPx);
}
