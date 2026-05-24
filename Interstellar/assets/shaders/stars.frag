#version 330 core
out vec4 FragColor;

in float vBright;

uniform vec3  u_Tint;
uniform float u_MinBrightness;
uniform float u_MaxBrightness;

void main() {
    // circular sprite
    vec2 uv = gl_PointCoord * 2.0 - 1.0;
    float r2 = dot(uv, uv);
    if (r2 > 1.0) discard;

    // gentle rim so points look crisp
    float rim = smoothstep(1.0, 0.94, sqrt(r2));

    float brightness = mix(u_MinBrightness, u_MaxBrightness, clamp(vBright, 0.0, 1.0));
    vec3 color = u_Tint * brightness * rim;
    FragColor = vec4(color, 1.0);
}
