#type vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex;

out vec2 vTex;

void main() {
    vTex = aTex;
    gl_Position = vec4(aPos, 1.0);
}

#type fragment
#version 330 core

in vec2 vTex;
out vec4 FragColor;

uniform sampler2D u_Texture;

void main() {
    FragColor = texture(u_Texture, vTex);
}
