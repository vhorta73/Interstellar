#version 330 core
out vec4 FragColor;

void main() {
    // Soft round star using gl_PointCoord
    vec2 uv = gl_PointCoord * 2.0 - 1.0;     // [-1,1]^2
    float r2 = dot(uv, uv);                  // radial falloff
    float alpha = smoothstep(1.0, 0.6, r2);  // 1 at center -> 0 at edge
    FragColor = vec4(vec3(1.0), alpha);
}
