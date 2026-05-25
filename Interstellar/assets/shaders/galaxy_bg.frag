#version 330 core

in vec3 vRayDir;
out vec4 FragColor;

uniform vec3  u_CamPos;
uniform vec3  u_GalCenter;
uniform vec3  u_GalRight;       // galaxy-plane X axis (orientation column 0)
uniform vec3  u_GalFwd;         // galaxy-plane Y axis (orientation column 1)
uniform vec3  u_GalNormal;      // disk normal       (orientation column 2)
uniform float u_RadialScale;    // sigma_r  (km)
uniform float u_VerticalScale;  // sigma_h  (km)
uniform float u_CoreRadius;     // sigma_c  (km)
uniform float u_CoreBoost;      // bulge multiplier
uniform float u_Brightness;     // overall tuning knob

void main() {
    vec3  dir     = normalize(vRayDir);
    float maxDist = u_RadialScale * 6.0;
    float stepKm  = maxDist / 64.0;

    float diskAccum  = 0.0;
    float bulgeAccum = 0.0;

    // Ray-march 64 steps through the galaxy density field
    for (int i = 1; i <= 64; i++) {
        float t = stepKm * float(i);
        vec3  p = (u_CamPos + dir * t) - u_GalCenter;

        float rx = dot(p, u_GalRight);
        float ry = dot(p, u_GalFwd);
        float h  = abs(dot(p, u_GalNormal));

        float r2 = rx * rx + ry * ry;

        float inv2sr2 = 1.0 / (2.0 * u_RadialScale  * u_RadialScale);
        float inv2sc2 = 1.0 / (2.0 * u_CoreRadius   * u_CoreRadius);

        float radial   = exp(-r2 * inv2sr2);
        float vertical = exp(-h  / max(u_VerticalScale, 1.0));
        float core     = exp(-r2 * inv2sc2);

        float disk = radial * vertical;
        diskAccum  += disk         * stepKm;
        bulgeAccum += disk * core  * stepKm;
    }

    // Normalise so that a tangential midplane view gives I ~ 0.6 * u_Brightness
    float I = diskAccum  / u_RadialScale * u_Brightness;
    float B = bulgeAccum / u_RadialScale * u_Brightness * u_CoreBoost * 0.15;

    // Disk: blue-white (hot OB-star light and combined Population I)
    // Bulge: warm yellow-orange (old Population II stars)
    vec3 diskCol  = vec3(0.72, 0.82, 1.00);
    vec3 bulgeCol = vec3(1.00, 0.88, 0.65);

    vec3  col   = diskCol * I + bulgeCol * B;
    float alpha = clamp(I * 1.5, 0.0, 0.92);

    FragColor = vec4(col, alpha);
}
