#version 330 core

layout(location = 0) in vec3 a_Pos;   // star world position

uniform mat4  u_VP;        // view-projection
uniform vec3  u_CamPos;    // camera world position

// perspective point-size control
uniform float u_PointScale; // set per-frame: (H/2)/tan(fov/2) * basePx

// size clamps (pixels)
uniform float u_MinPx;
uniform float u_MaxPx;

// brightness window (NEAR < FAR)
uniform float u_BrightNear;   // fully bright at or inside this
uniform float u_BrightFar;    // fades to min beyond this

out float vBright; // to fragment

void main() {
    vec4 clip = u_VP * vec4(a_Pos, 1.0);
    gl_Position = clip;

    // Distance to camera
    float d = distance(u_CamPos, a_Pos);

    // Brightness: 0 at >= Far, 1 at <= Near
    vBright = 1.0 - smoothstep(u_BrightNear, u_BrightFar, d);

    // Perspective-correct point size (in pixels)
    float invW = 1.0 / max(1e-6, clip.w);        // guard divide-by-zero
    float sizePx = u_PointScale * invW;          // shrinks with distance
    gl_PointSize = clamp(sizePx, u_MinPx, u_MaxPx);
}
