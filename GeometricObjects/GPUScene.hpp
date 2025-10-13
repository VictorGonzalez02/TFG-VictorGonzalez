#pragma once

#include <vector>
#include <memory>

#include "GeometricObjects/Object.hpp"
#include "GeometricObjects/Mesh.hpp"
#include "GeometricObjects/Cub.hpp"
#include "GeometricObjects/GPUMaterial.hpp"

using namespace std;

class GPUScene : public GPUConnectable
{
public:

    GPUScene();
    
    virtual ~GPUScene();
   

    // Vector d'objectes continguts a l'escena
    std::vector<shared_ptr<Object>> objects;
    std::vector<shared_ptr<Object>> getObjects() {return objects;}
    void   addObject(shared_ptr<Object> obj);

    void setMaterial(shared_ptr<GPUMaterial> m);
    void lastObjectToGPU(GLuint program);
    
    void toGPU (GLuint p) override;
    void draw() override;  

    void toGPUTexture(GLuint p);

    bool isTextured() { 
        for (auto obj : objects) {
            if (obj->isTextured()) return true;
        }
        return false;
    };

    void rebindTexture() {
        for (auto obj : objects) {
            if (obj->isTextured()) obj->rebindTexture();
        }
    };

    void initTextureGL(const char *nomTextura) {
        for (auto obj : objects) {
            if (obj->isTextured() && obj == objects.at(objects.size()-1)) obj->initTextureGL(nomTextura);
        }
    };

    void aplicaTG(glm::mat4 m)
    {
        GLuint modelMatrixLoc = glGetUniformLocation(program, "modelMatrix");
        glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(m));

        //Invertim la matriu model per crear la matriu normal i la passem a GPU
        mat4 normalMatrix = transpose(inverse(m));
        GLuint normalMatrixLoc = glGetUniformLocation(program, "normalMatrix");
        glUniformMatrix4fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }   

    void setTGLastObject(glm::mat4 m) {
        objects.back().get()->aplicaTG(m);
    }
};

