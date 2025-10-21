#include "GPUScene.hpp"

GPUScene::GPUScene()
{
    objects.clear();
    pointCloud = new PointCloud(1, "resources/OBJFiles/1_0_0.ply");
   
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

void GPUScene::addPointCloud(PointCloud* p){
    pointCloud = p;
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

void GPUScene::toGPU_PointCloud(GLuint p, Shader* s){
    program = p;
    this->pointCloud->bind3DTexture();
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

void GPUScene::drawPointCloud(Shader* s){
    this->pointCloud->render(s);
}

void GPUScene::setMaterial(shared_ptr<GPUMaterial> m) {
    // Modify the material to the last added object
    if (objects.size()>0) {
        objects.at(objects.size()-1)->setMaterial(m);
    }
}
