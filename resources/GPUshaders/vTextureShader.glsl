#version 330 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vColor;
layout (location = 2) in vec2 vCoordTexture;
layout (location = 3) in vec4 vNormal;

uniform mat4 modelMatrix;
uniform mat4 normalMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lookFrom;
uniform vec3 globalAmbientLight;

out vec3 v_Position;
out vec3 normal;
out vec2 v_texcoord;

void main()
{
   // Calculate world space position
    gl_Position = projectionMatrix*viewMatrix*modelMatrix * vPosition;

    // Pas de les coordenades de textura al fragment shader
    v_texcoord = vCoordTexture;
    // El valor dels colors i les coordenades de textura s'interpolaran automaticament
    // en els pixels interiors a les cares dels polígons

    v_Position = vec3(modelMatrix * vPosition);
    normal = mat3(normalMatrix) * vec3(vNormal);
   
}