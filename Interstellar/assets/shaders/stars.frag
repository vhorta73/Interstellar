#version 330 core

in float vBright;
in float vCoreFrac;
in vec3  vColor;

out vec4 FragColor;

uniform float u_MinBrightness;
uniform float u_MaxBrightness;

void main() {
    // uv in [-1,1] from sprite centre
    vec2  uv = gl_PointCoord * 2.0 - 1.0;
    float r  = length(uv);
    if (r > 1.0) discard;

    // rn=1 at the physical core edge; >1 in the halo region
    float rn = r / max(vCoreFrac, 0.01);

    // Tight Gaussian core
    float core = exp(-rn * rn * 2.0);

    // Wide soft halo — falls to near-zero at sprite edge
    float halo = exp(-rn * rn * 0.18) * 0.12;

    // 4-axis diffraction spikes (cross pattern), activated on resolvable stars
    float ax = abs(uv.x), ay = abs(uv.y);
    float spikeWidth = 0.003 + vCoreFrac * 0.012;
    float spikeH = exp(-ay * ay / spikeWidth) * exp(-ax * ax * 0.35);
    float spikeV = exp(-ax * ax / spikeWidth) * exp(-ay * ay * 0.35);
    float spike  = (spikeH + spikeV) * 0.09 * clamp(vBright * 2.5, 0.0, 1.0);

    float alpha = clamp((core + halo + spike) * vBright, 0.0, 1.0);

    float brightness = mix(u_MinBrightness, u_MaxBrightness, clamp(vBright, 0.0, 1.0));
    FragColor = vec4(vColor * brightness, alpha);
}
