#version 330 core

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
uniform mat4 projectionMatrix;
uniform vec3 lookFrom;
uniform vec3 globalAmbientLight;

in vec3 v_Position;
in vec3 normal;
out vec4 colorOut;

void main() {
    vec3 N = normalize(normal);
    vec3 L = normalize(lightArr[0].pos - v_Position);

    float intensity = dot(L, N);

    if (intensity > 0.95)
		colorOut = vec4(material.Kd,1.0);
	else if (intensity > 0.5)
		colorOut = vec4(material.Kd,1.0) * vec4(0.6,0.6,0.6,1.0);
	else if (intensity > 0.25)
		colorOut = vec4(material.Kd,1.0) * vec4(0.4,0.4,0.4,1.0);
	else
		colorOut = vec4(material.Kd,1.0) * vec4(0.2,0.2,0.2,1.0);
        
}