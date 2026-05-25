#version 330 core

layout(location = 0) in vec2 a_Pos; // fullscreen quad in NDC [-1,1]

uniform mat4 u_InvVP;
uniform vec3 u_CamPos;

out vec3 vRayDir;

void main() {
    gl_Position = vec4(a_Pos, 0.9999, 1.0);

    // Reconstruct a world-space point on the far plane, then form the ray direction
    vec4 world = u_InvVP * vec4(a_Pos, 1.0, 1.0);
    world /= world.w;
    vRayDir = world.xyz - u_CamPos;
}
