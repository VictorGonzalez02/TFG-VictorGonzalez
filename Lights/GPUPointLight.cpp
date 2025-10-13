#include "GPUPointLight.hpp"
#include <iostream>

//Struct per a passar dades a la GPU
struct {
    GLuint Ia;
    GLuint Id;
    GLuint Is;
    GLuint pos;
    GLuint a;
    GLuint b;
    GLuint c;
} Dades;

GPUPointLight::GPUPointLight(vec3 posicio, vec3 Ia, vec3 Id, vec3 Is): GPULight(Ia, Id, Is) {
    this->pos = posicio;
    this->a = 0.0f;
    this->b = 0.0f;
    this->c = 0.0f;
}

GPUPointLight::GPUPointLight(vec3 posicio, vec3 Ia, vec3 Id, vec3 Is, float a, float b, float c): GPULight(Ia, Id, Is) {
    this->pos = posicio;
    this->a = a;
    this->b = b;
    this->c = c;
}

GPUPointLight::GPUPointLight(vec3 posicio, vec3 Ia, vec3 Id, vec3 Is, float a, float b, float c, bool isEnabled): GPULight(Ia, Id, Is, isEnabled) {
    this->pos = posicio;
    this->a = a;
    this->b = b;
    this->c = c;
}

vec3 GPUPointLight::vectorL(vec3 point) {
    return normalize(pos - point);
}

float GPUPointLight::attenuation(vec3 point) {
    if (abs(a)<DBL_EPSILON && abs(b)<DBL_EPSILON  && abs(c)<DBL_EPSILON) {
        //Si tots els coeficients son 0 considerem que no hi ha atenuacio
        return 1.0f;
    }
    //Calculem la distancia entre el punt i la posicio de la llum
    float d = distance(point, pos);
    return 1.0f/(c*d*d + b*d + a);
}

float GPUPointLight::distanceToLight(vec3 point) {
    return distance(point, pos);
}

void GPUPointLight:: toGPU(GLuint pr) {
    // TO DO: PAS 2.2. enviar la posició de la llum puntual a la GPU  i els coeficients d'atenuació
    // Cal obtenir l'identificador de la variable uniform de la GPU
    // i actualitzar el seu valor amb la posició de la llum puntual
    
    program = pr;
    Dades.Ia = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].Ia").c_str());
    Dades.Id = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].Id").c_str());
    Dades.Is = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].Is").c_str());
    Dades.pos = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].pos").c_str());
    Dades.a = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].a").c_str());
    Dades.b = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].b").c_str());
    Dades.c = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].c").c_str());
    glUniform3fv(Dades.Ia, 1, &Ia[0]);
    glUniform3fv(Dades.Id, 1, &Id[0]);
    glUniform3fv(Dades.Is, 1, &Is[0]);
    glUniform3fv(Dades.pos, 1, &pos[0]);
    glUniform1f(Dades.a, a);
    glUniform1f(Dades.b, b);
    glUniform1f(Dades.c, c);
}

void GPUPointLight::updateToGPU(int index) {
    // TO DO: PAS 2.3: actualitzar les propietats de la llum puntual a la GPU
    // Cal obtenir els identificadors de les variables uniform de la GPU de la llum amb index "index"
    // i actualitzar els seus valors amb les propietats de la llum puntual
    Dades.Ia = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].Ia").c_str());
    Dades.Id = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].Id").c_str());
    Dades.Is = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].Is").c_str());
    Dades.pos = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].pos").c_str());
    Dades.a = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].a").c_str());
    Dades.b = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].b").c_str());
    Dades.c = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].c").c_str());
    glUniform3fv(Dades.Ia, 1, &Ia[0]);
    glUniform3fv(Dades.Id, 1, &Id[0]);
    glUniform3fv(Dades.Is, 1, &Is[0]);
    glUniform3fv(Dades.pos, 1, &pos[0]);
    glUniform1f(Dades.a, a);
    glUniform1f(Dades.b, b);
    glUniform1f(Dades.c, c);
}
