#version 450

layout(local_size_x = 16, local_size_y = 16) in;

// Binding 0 = positions
layout(std430, binding = 0) readonly buffer Positions { vec4 positions[]; };
// Binding 1 = colors
layout(std430, binding = 1) readonly buffer Colors { vec4 colors[]; };
// Output framebuffer (emulated 64-bit with 2x32-bit)
layout(std430, binding = 2) buffer Framebuffer { uvec2 framebuffer[]; };

uniform uint imageWidth;
uniform uint imageHeight;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uint packRGB(vec3 c) {
    uvec3 rgb = uvec3(clamp(c * 255.0, 0.0, 255.0));
    return (rgb.r << 16) | (rgb.g << 8) | rgb.b;
}

// Compare two uvec2 "64-bit" values
bool less64(uvec2 a, uvec2 b) {
    return (a.y < b.y) || (a.y == b.y && a.x < b.x);
}

// Emulated atomicMin for 64-bit (two 32-bit atomics)
void atomicMin64(uint idx, uvec2 newVal)
{
    for (;;) {
        uvec2 old = framebuffer[idx];
        if (!less64(newVal, old)) return;

        // Try atomic swap on low word
        uint oldLow = old.x;
        uint swapped = atomicCompSwap(framebuffer[idx].x, oldLow, newVal.x);
        if (swapped == oldLow) {
            // Now try high word (depth)
            uint oldHigh = old.y;
            uint swappedHigh = atomicCompSwap(framebuffer[idx].y, oldHigh, newVal.y);
            if (swappedHigh == oldHigh)
                return;
        }
        // If race detected, loop again
    }
}


void main() {

    uint i = gl_GlobalInvocationID.x + 
             gl_GlobalInvocationID.y * gl_NumWorkGroups.x * gl_WorkGroupSize.x;
    if (i >= positions.length()) return;

    vec4 pos = projectionMatrix*viewMatrix*modelMatrix * positions[i];
    if (pos.w <= 0.0) return; // behind camera

    vec3 ndc = pos.xyz / pos.w;
    if (abs(ndc.x) > 1.0 || abs(ndc.y) > 1.0) return; // off-screen

    // convert to pixel coordinates
    uint x = uint((ndc.x * 0.5 + 0.5) * float(imageWidth));
    uint y = uint((ndc.y * 0.5 + 0.5) * float(imageHeight));
    if (x >= imageWidth || y >= imageHeight) return;

    uint pixelID = x + y * imageWidth;
    uint depthKey = floatBitsToInt(ndc.z);
    uint rgb = packRGB(colors[i].rgb);

    uvec2 point = uvec2(rgb, depthKey);
    atomicMin64(pixelID, point);
}
