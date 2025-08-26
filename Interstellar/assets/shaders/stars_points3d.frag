#version 330 core
out vec4 FragColor;

uniform vec3  u_Tint;        // star color
uniform float u_Brightness;  // 1..2

void main() {
    // round point sprite with feathered edge & additive-ish look
    vec2 uv = gl_PointCoord * 2.0 - 1.0;       // [-1,1]
    float r = length(uv);
    float core = 1.0 - smoothstep(0.0, 1.0, r); // 1 at center -> 0 at edge
    float glow = smoothstep(1.0, 0.0, r) * 0.25;
    float alpha = clamp(core + glow, 0.0, 1.0);

    vec3 color = u_Tint * u_Brightness;
    FragColor = vec4(color, alpha);
}
