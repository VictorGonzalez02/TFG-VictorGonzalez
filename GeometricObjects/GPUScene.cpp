#include "GPUScene.hpp"

GPUScene::GPUScene()
{
    objects.clear();
   
}
/**
 * @brief Scene::~Scene
 */
GPUScene::~GPUScene() {
    objects.clear();
}

/**
 * @brief GPUScene::addObject
 * @param obj
 */
void GPUScene::addObject(shared_ptr<Object> obj) {
    objects.push_back(obj);
}

/**
 * @brief GPUScene::toGPU
 */
void GPUScene::toGPU(GLuint p) {
    program = p;
    for(unsigned int i=0; i < objects.size(); i++){
        objects.at(i)->toGPU(p);
    }
}

void GPUScene::toGPUTexture(GLuint p) {
    this->program = p;
    for (unsigned int i = 0; i < objects.size(); i++) {
        if (objects.at(i)->isTextured()) objects.at(i)->toGPUTexture(program);
    }
};

void GPUScene::lastObjectToGPU(GLuint program) {
    if (objects.size()>0) {
        objects.at(objects.size()-1)->toGPU(program);
    }
}
/**
 * @brief GPUScene::draw
 */
void GPUScene::draw() {
    for(unsigned int i=0; i < objects.size(); i++){
        objects.at(i)->draw();
    }
}

void GPUScene::setMaterial(shared_ptr<GPUMaterial> m) {
    // Modify the material to the last added object
    if (objects.size()>0) {
        objects.at(objects.size()-1)->setMaterial(m);
    }
}
