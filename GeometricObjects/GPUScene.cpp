#include "GPUScene.hpp"

GPUScene::GPUScene()
{
    objects.clear();
    pointCloud = nullptr;
    pointCloud = new PointCloud(1, "/home/victor/TFG-VictorGonzalez/resources/OBJFiles/Goat Morton.ply");
   
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
    if(this->pointCloud != nullptr){
        this->pointCloud->toGPU(p);
    }
}

void GPUScene::toGPU_PointCloud(GLuint p){
    program = p;
    //this->pointCloud->bind3DTexture();
    if(this->pointCloud != nullptr){
        this->pointCloud->toGPU(p);
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
    if(this->pointCloud != nullptr){
        this->pointCloud->draw();
    }
}

void GPUScene::drawPointCloud(){
    /*glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    this->pointCloud->render(s);*/
    this->pointCloud->draw();
}

void GPUScene::setMaterial(shared_ptr<GPUMaterial> m) {
    // Modify the material to the last added object
    if (objects.size()>0) {
        objects.at(objects.size()-1)->setMaterial(m);
    }
}
