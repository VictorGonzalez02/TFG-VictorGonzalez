#include "Lights/GPULight.hpp"

static struct {
    GLuint Ia;
    GLuint Id;
    GLuint Is;
} Dades;

void GPULight::toGPU(GLuint p) {
    // TO DO: enviar les propietats de Ia, Id i Is a la GPU. Pas 2.2
    program = p;
    Dades.Ia = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].Ia").c_str());
    Dades.Id = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].Id").c_str());
    Dades.Is = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].Is").c_str());
    glUniform3fv(Dades.Ia, 1, &Ia[0]);
    glUniform3fv(Dades.Id, 1, &Id[0]);
    glUniform3fv(Dades.Is, 1, &Is[0]);

}

void GPULight::updateToGPU(int index) {
    // TO DO: actualitzar les propietats de la llum a la GPU. Pas 2.3
    // Cal obtenir els identificadors de les variables uniform de la GPU
    // i actualitzar els seus valors amb les propietats de la llum

    Dades.Ia = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].Ia").c_str());
    Dades.Id = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].Id").c_str());
    Dades.Is = glGetUniformLocation(program, ("lightArr[" + std::to_string(index) + "].Is").c_str());
    glUniform3fv(Dades.Ia, 1, &Ia[0]);
    glUniform3fv(Dades.Id, 1, &Id[0]);
    glUniform3fv(Dades.Is, 1, &Is[0]);
    
}
