#include "GPUMaterial.hpp"

struct {
    GLuint Ka;
    GLuint Kd;
    GLuint Ks;
    GLuint shininess;
    GLuint opacity;
} Dades;

GPUMaterial::GPUMaterial(): Ka(0.2f), Kd(1.0f), Ks(0.8f) {
    shininess = 100.0f;
    opacity = 1;
};
GPUMaterial::GPUMaterial(vec3 d):  Ka(0.2f), Kd(d), Ks(0.8f) {
    shininess = 100.0f;
    opacity = 1;
};

GPUMaterial::GPUMaterial(vec3 a, vec3 d, vec3 s, float shininess):  Ka(a), Kd(d), Ks(s), shininess(shininess) {
    opacity = 1;
};

void GPUMaterial::toGPU(GLuint program) {
     // Set material properties to GPU
    // TO DO: PAS 3.1: Enviar les propietats del material a la GPU

    this->program = program;
    Dades.Ka = glGetUniformLocation(program, "material.Ka");
    Dades.Kd = glGetUniformLocation(program, "material.Kd");
    Dades.Ks = glGetUniformLocation(program, "material.Ks");
    Dades.shininess = glGetUniformLocation(program, "material.shininess");
    Dades.opacity = glGetUniformLocation(program, "material.opacity");
    glUniform3fv(Dades.Ka, 1, &Ka[0]);
    glUniform3fv(Dades.Kd, 1, &Kd[0]);
    glUniform3fv(Dades.Ks, 1, &Ks[0]);
    glUniform1f(Dades.shininess, shininess);
    glUniform1f(Dades.opacity, opacity);

}