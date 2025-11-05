#version 450
layout(std430, binding = 2) readonly buffer Framebuffer { uvec2 fb[]; };
uniform uint imageWidth;

in vec2 uv;
out vec4 fragColor;

vec3 unpackRGB(uint pack) {
    return vec3(
        float((pack >> 16) & 0xFFu),
        float((pack >> 8) & 0xFFu),
        float(pack & 0xFFu)
    ) / 255.0;
}

void main() {
    uint x = uint(gl_FragCoord.x);
    uint y = uint(gl_FragCoord.y);
    uint pixelID = x + y * imageWidth;

    // fb[pixelID].x = rgb, fb[pixelID].y = depth
    uvec2 val = fb[pixelID];
    uint rgb = val.x;   // read packed color
    fragColor = vec4(unpackRGB(rgb), 1.0);
}
