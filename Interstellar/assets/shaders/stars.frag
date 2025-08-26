#version 330 core
out vec4 FragColor;

in float vBright;

uniform vec3  u_Tint;            // usually vec3(1)
uniform float u_MinBrightness;   // faint floor, e.g. 0.06
uniform float u_MaxBrightness;   // bright cap, e.g. 1.25

void main() {
    // Crisp circular sprite (no blur, no square)
    vec2 uv = gl_PointCoord * 2.0 - 1.0; // [-1,1]
    float r2 = dot(uv, uv);
    if (r2 > 1.0) discard;

    // Slight edge emphasis to avoid perfectly flat disks
    float rim = smoothstep(1.0, 0.94, sqrt(r2));

    float brightness = mix(u_MinBrightness, u_MaxBrightness, clamp(vBright, 0.0, 1.0));
    vec3 color = u_Tint * brightness * rim;
    FragColor = vec4(color, 1.0);
}
