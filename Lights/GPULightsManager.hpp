#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GPULight.hpp"
#include "GPUPointLight.hpp"


class GPULightsManager  {
    private:
        // Vector de llums contingudes la mon
        vector<shared_ptr<GPUPointLight>> lights;
        // Llum ambient global:
        vec3  ambientLight;

    public:
        GPULightsManager()  {
            ambientLight = vec3(0.2f, 0.2f, 0.2f);
        }
        
        void setAmbientLight(vec3 a) {
            ambientLight = a;
        }

        void toGPU(GLuint program) {
            if (program == 0) return;

            // TO DO: Enviar tota la informació de la il·luminació a la GPU
            
            // TO DO: Enviar només les llums que estan actives a la GPU 
            
            // TO DO: Posar en el shader el nombre de llums actives

            for(int i = 0; i < lights.size(); i++){
                lights.at(i)->toGPU(program);
            }
            ambientLightToGPU(program, ambientLight);

            GLuint num = glGetUniformLocation(program, "numLights");
            glUniform1i(num, lights.size());
            
        }
        

        void addLight(shared_ptr<GPUPointLight> l) {
            l->setIndex(lights.size());
            lights.push_back(l);
        }

        void ambientLightToGPU(GLuint program, vec3 a) {
            GLuint globalAmbientLight;
            globalAmbientLight = glGetUniformLocation(program, "globalAmbientLight");
            glUniform3fv(globalAmbientLight, 1, &a[0]);
            // TO DO: Enviar la llum ambient global a la GPU i revisar des d'on es crida aquest mètode
        }

        void updateAllLights(GLuint program, vector<GPUPointLight> &lightsNew) {
            if (program == 0) return;
            
            lights.clear();

            for (int i = 0; i < lightsNew.size(); i++) {
                if(lightsNew[i].isEnabled()){
                    lights.push_back(make_shared<GPUPointLight>(lightsNew[i].getPos(), lightsNew[i].getIa(), lightsNew[i].getId(), lightsNew[i].getIs(), 0.0f, 0.0f, 1.0f, lightsNew[i].isEnabled()));
                    lights[i]->setIndex(i);
                }
            }

            // TO DO Fitxa 2: Cal enviar totes les llums a la GPU des d'aquest mètode?
            for (int i = 0; i < lights.size(); i++){
                    lights.at(i)->toGPU(program);
            }

            GLuint num = glGetUniformLocation(program, "numLights");
            glUniform1i(num, lights.size());
        }

        void updateSingleLight(GLuint program, GPUPointLight &pl, int index) {
            if (program == 0) return;
            
            lights[index] = make_shared<GPUPointLight>(pl.getPos(), pl.getIa(), pl.getId(), pl.getIs(), 0.0f, 0.0f, 1.0f);
            lights[index]->setIndex(index);
            lights[index]->toGPU(program);
            
            // TO DO Fitxa 2: Cal només enviar la llum que està a "index" del vector de llums  a la GPU des d'aquest mètode?
            
        }


};