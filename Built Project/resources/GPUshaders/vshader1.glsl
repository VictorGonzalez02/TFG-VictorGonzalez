#version 330 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 color;

void main() {
    gl_Position = projectionMatrix*viewMatrix*modelMatrix * vPosition;
    color = vColor;
}