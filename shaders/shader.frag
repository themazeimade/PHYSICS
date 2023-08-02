#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec4 outColor;

void main() { 
    vec2 alpha;
    alpha.x = (texcoord.x*2);
    alpha.y = (texcoord.y*2);
    alpha -= 1.0;

    float fade = 0.005;

    float distance = 1.0 - length(alpha);

    distance = smoothstep(0.0,fade, distance);
    outColor = vec4(fragColor, distance);
}
