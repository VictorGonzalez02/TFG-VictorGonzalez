#pragma once

#include <random> // Llibreria per generar números aleatoris
#include <memory>

#include "GeometricObjects/GPUScene.hpp"
#include "Lights/GPULightsManager.hpp"
#include "GPUCamera.hpp"
#include "GPUConfig.hpp"

#include <stdlib.h>
#include <iostream>

class GPUWorld: public GPUConnectable
{
public:

    // Escena que conte els objectes
    shared_ptr<GPUScene> scene;
    // Lights of the scene
    shared_ptr<GPULightsManager> lightsManager;
    // Camera 
    shared_ptr<GPUCamera> camera;

 
    // Configuracio de la visualitzacio
    shared_ptr<GPUConfig> setup;

    GPUWorld();

    virtual ~GPUWorld() {};

    void setConfig(shared_ptr<GPUConfig> s) { setup = s; refreshProperties();}
    void setCamera(shared_ptr<GPUCamera> c) { camera = c; }    
    void setLightManager(shared_ptr<GPULightsManager> l) { lightsManager = l; }
    
    
    void addLight(shared_ptr<GPUPointLight> l) { lightsManager->addLight(l); }
    void addObject (shared_ptr<Object> obj) { scene->addObject(obj);}
    void addPointCloud(PointCloud* p){ scene->addPointCloud(p);}

    void updateAmbientLight(GLuint program, vec3 ambient) { 
        lightsManager->ambientLightToGPU(program, ambient);
    };
    void updateAllLights(GLuint program, vector<GPUPointLight> lights) { 
        lightsManager->updateAllLights(program, lights); 
    }
   
    void updateSingleLight(GLuint program, GPUPointLight &light, int index) { 
        lightsManager->updateSingleLight(program, light, index);

    };
    void setScene(shared_ptr<GPUScene> s) { scene = s; };
    
    void updateCamera(GLuint program, vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float dant, float dpost) {
        camera->updateCamera(lookfrom, lookat, vup, vfov, dant, dpost);
        camera->toGPU(program);
    };
    void updateMaterial(GLuint program, vec3 ambient, vec3 diffuse, vec3 specular, float shininess) {
        scene->setMaterial(make_shared<GPUMaterial>(ambient, diffuse, specular, shininess));
        scene->lastObjectToGPU(program);
    };

    void lastObjectToGPU(GLuint program) {
        scene->lastObjectToGPU(program);
    }
    void toGPU(GLuint program) override;
    void toGPU_PointCloud(GLuint program);

    void draw() override {
        scene->draw();
    };
    void drawPointCloud(){
        scene->drawPointCloud();
    }

    int getNumPoints(){
        return scene->getNumPoints();
    }

    void toGPUTexture(GLuint program);
    bool isTextured() { return scene->isTextured(); };
    void rebindTexture() { scene->rebindTexture(); };
    void initTextureGL(const char *nomTextura) { scene->initTextureGL(nomTextura); };

    void refreshProperties();
    void aplicaTG(glm::mat4 m);
    void setTGLastObject(glm::mat4 m) { 
        scene->setTGLastObject(m);
    }
};  

