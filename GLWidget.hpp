#pragma once

#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <memory>
#include <filesystem>

#include "Utilities/GPUShader.hpp"
#include "Utilities/Shader.h"
#include "GPUConfig.hpp"
#include "GPUWorld.hpp"

using namespace glm;

class GLWidget 
{
public:
    GLWidget(int w, int h);
    ~GLWidget();

    // Funcions bàsiques de l'OpenGL
    void initializeGL();
    void paintGL();

    // Control de les rotacions
    void  setXRotation(float angle);
    void  setYRotation(float angle);
    void  setZRotation(float angle);
    float getXRotation() const { return xRot; }
    float getYRotation() const { return yRot; }
    float getZRotation() const { return zRot; }
    
    // Managament dels shaders
    void   initShadersGPU(); // inicialitza tots els parells de shaders 
    void   activateShader(const char *typeShader, const char *nameTexture);
    
    // Mouse control
    void mousePressEvent(GLFWwindow* window, int button, int action, int mods);
    void mouseMoveEvent(GLFWwindow* window, double xpos, double ypos);
    

    // Configuration access
    GPUConfig& getConfig() { return config; }
    
    // GPUMaterial update
    void updateMaterial();
    
    // Camera update
    void updateCamera();

    //GL_DEPTH_TEST update
    void updateDepthTest(bool b);
    
    // Lights update
    void updateAllLights();
    void updateGlobalAmbientLight();
    void updateSingleLight(int index);
    
    // Render mode
    void setRenderMode(int mode);
    int  getRenderMode() const;
    
    // Background
    void updateBackground();


    // Funcions de càrrega d'objectes
    void loadObject(const char* filename) ;
    void loadPointCloud(const char* filename);
    void addCube();
    void reset();

    void setupBuffer();
    void computePass();
    void displayPass();

private:

    // Objectes de l'escena
    shared_ptr<GPUWorld> world;
    

    // Estat de les rotacions i translacions
    float xRot; // en graus
    float yRot;
    float zRot;

    float xTra;
    float yTra;
    float zTra;
    
    glm::mat4 transform;

    // Shaders

    shared_ptr<GPUShader> shaderGL_Points;
    shared_ptr<GPUShader> shaderZTest;
    shared_ptr<GPUShader> shaderZTest_No_Depth;
    shared_ptr<GPUShader> shaderZTestDisplay;
    shared_ptr<GPUShader> program; 

    std::vector<Shader*> shaders;       

    // Mouse control
    bool   mousePressed;
    double lastMouseX;
    double lastMouseY;
    
    // Configuration
    GPUConfig config;

    // Funcions de suport a l'OpenGL
    void setupOpenGLFeatures();

    // Funcions de construcció dels vertexs dels objectes de l'escena a passar a la GPU
    void  initWorld();


    // Normalitza l'angle en radians entre [0, 2π)
    float normalizeAngle(float angle) {
        return angle - 2.0f * M_PI * floor(angle / (2.0f * M_PI));
    }
};
