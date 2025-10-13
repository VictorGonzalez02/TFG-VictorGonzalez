#version 330 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vColor;

struct Light {
    vec3 Ia;
    vec3 Id;
    vec3 Is;
    vec3 pos;
    float a;
    float b;
    float c;
};

uniform Light lightArr[8];

uniform int numLights;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 globalAmbientLight;

out vec4 color;

void main() {
    // Calculate world space position
    gl_Position = projectionMatrix*viewMatrix*modelMatrix * vPosition;
    color = vColor;
    //color = vec4(globalAmbientLight[0], globalAmbientLight[1], globalAmbientLight[2], 0);
    //color = vec4(lightArr[0].Id[0], lightArr[0].Id[1], lightArr[0].Id[2], 0);
    //color = vec4(0,0,0,0);
    //for(int i = 0; i < numLights; i++){
        //color += vec4(lightArr[i].Id[0], lightArr[i].Id[1], lightArr[i].Id[2], 0);
    //}
}