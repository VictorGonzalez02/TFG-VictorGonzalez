#version 330 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vColor;
layout (location = 2) in vec4 vCoordTexture;
layout (location = 3) in vec4 vNormal;

uniform mat4 modelMatrix;
uniform mat4 normalMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 v_Position;
out vec3 normal;

void main() {
    // Calculate world space position
    gl_Position = projectionMatrix*viewMatrix*modelMatrix * vPosition;
    v_Position = vec3(modelMatrix * vPosition);
    normal = mat3(normalMatrix) * vec3(vNormal);
}