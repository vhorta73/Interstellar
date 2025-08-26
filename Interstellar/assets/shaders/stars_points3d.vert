#version 330 core
layout(location = 0) in vec3 a_Position;

uniform mat4  u_VP;              // view-projection
uniform vec3  u_CamPos;          // camera world position
uniform float u_PointBase;       // base pixel size (e.g. 6..12)
uniform float u_SizeAtten;       // 0=no attenuation, 1=1/d falloff

out float vDist;

void main() {
    vDist = distance(a_Position, u_CamPos);

    // Simple size attenuation (avoid huge flicker):
    float px = u_PointBase;
    if (u_SizeAtten > 0.0) {
        // 1/(epsilon + distance)
        px = u_PointBase / (0.15 + vDist);
        px = clamp(px, 1.0, 96.0);
    }
    gl_PointSize = px;

    gl_Position = u_VP * vec4(a_Position, 1.0);
}
