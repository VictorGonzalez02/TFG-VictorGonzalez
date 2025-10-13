#version 330 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vColor;
layout (location = 2) in vec4 vCoordTexture;
layout (location = 3) in vec4 vNormal;

struct Material {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shininess;
    float opacity;
};

uniform Material material;

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
uniform mat4 normalMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lookFrom;
uniform vec3 globalAmbientLight;
vec3 v_Position;
vec3 normal;

out vec4 color;

void main() {
    gl_Position = projectionMatrix*viewMatrix*modelMatrix * vPosition;
    v_Position = vec3(modelMatrix * vPosition);
    normal = mat3(normalMatrix) * vec3(vNormal);

    vec3 N = normalize(normal);
    vec3 V = normalize(lookFrom - v_Position);

    vec3 ambient = material.Ka * globalAmbientLight;
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);

    for(int i = 0; i < numLights; i++){
        ambient += material.Ka * lightArr[i].Ia;

        vec3 L = normalize(lightArr[i].pos - v_Position);
        vec3 H = normalize(L + V);

        float cos = max(dot(L, N), 0.0f);
        float cos2 = 0.0f;
        if(cos > 0.0f){
            cos2 = pow(max(dot(N, H), 0.0f), material.shininess);
        }

        diffuse += material.Kd * lightArr[i].Id * cos;
        specular += material.Ks * lightArr[i].Is * cos2;

    }
    color = vec4(ambient[0]+diffuse[0]+specular[0], ambient[1]+diffuse[1]+specular[1], ambient[2]+diffuse[2]+specular[2], 1.0f);
}