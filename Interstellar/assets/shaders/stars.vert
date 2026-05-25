#version 330 core

layout(location = 0) in vec3  a_Pos;
layout(location = 1) in float a_Radius;    // physical radius in km
layout(location = 2) in float a_Intensity; // [0,1] spectral / brightness seed

uniform mat4  u_VP;
uniform vec3  u_CamPos;
uniform float u_FocalLengthPx;
uniform float u_MinPx, u_MaxPx;
uniform float u_MaxDistKm;

out float vBright;
out float vCoreFrac;  // physical-core half-radius as fraction of sprite half-size
out vec3  vColor;

// Blackbody-inspired 5-segment spectral ramp.
// t=0 => deep red (M-class),  t=1 => blue-white (O/B-class).
// Distribution: most stars drawn with low intensity => M/K dominant, matching reality.
vec3 starColor(float t) {
    if (t < 0.18)
        return mix(vec3(1.00, 0.22, 0.04), vec3(1.00, 0.48, 0.18), t / 0.18);
    if (t < 0.35)
        return mix(vec3(1.00, 0.48, 0.18), vec3(1.00, 0.80, 0.42), (t - 0.18) / 0.17);
    if (t < 0.55)
        return mix(vec3(1.00, 0.80, 0.42), vec3(1.00, 1.00, 0.88), (t - 0.35) / 0.20);
    if (t < 0.75)
        return mix(vec3(1.00, 1.00, 0.88), vec3(0.88, 0.94, 1.00), (t - 0.55) / 0.20);
    return     mix(vec3(0.88, 0.94, 1.00), vec3(0.55, 0.72, 1.00), (t - 0.75) / 0.25);
}

void main() {
    float dist = distance(u_CamPos, a_Pos);

    if (dist > u_MaxDistKm) {
        gl_Position = vec4(2.0, 2.0, 0.0, 1.0);
        vBright   = 0.0;
        vCoreFrac = 0.0;
        vColor    = vec3(0.0);
        return;
    }

    gl_Position = u_VP * vec4(a_Pos, 1.0);

    // Physical angular diameter in pixels
    float angDiamPx = 2.0 * a_Radius / max(dist, 1e-4) * u_FocalLengthPx;

    // Sprite is 4x the physical core to accommodate halo; clamped to [2*minPx, maxPx]
    float spritePx = clamp(angDiamPx * 4.0, u_MinPx * 2.0, u_MaxPx);
    gl_PointSize = spritePx;

    // Fraction of the half-sprite radius that the physical core occupies
    vCoreFrac = clamp(angDiamPx / spritePx, 0.01, 1.0);

    float resolvedFrac = clamp(angDiamPx / u_MinPx, 0.0, 1.0);
    vBright = mix(a_Intensity, 1.0, resolvedFrac);

    vColor = starColor(a_Intensity);
}
