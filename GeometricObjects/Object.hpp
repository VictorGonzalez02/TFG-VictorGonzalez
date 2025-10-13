#pragma once

#include <iostream>
#include <memory>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "external/ImGuiFileDialog/stb/stb_image.h"

#include "GPUMaterial.hpp"
#include "Utilities/GPUConnectable.hpp"

using namespace std;
// Es la classe pare de tots els objectes que s'han de visualitzar.
// Es fa una classe Objectes que hereda de Hitable i ha d'implementar el metode intersection

class Object: public GPUConnectable {
  public:
    Object();
    virtual ~Object() {};

    void     setMaterial(shared_ptr<GPUMaterial> m);
    shared_ptr<GPUMaterial> getMaterial();
    void                    materiaToGPU();

    virtual void toGPU(GLuint program) override;
    void    draw() override;

    void   initTextureGL(const char *nomTextura);
    void   rebindTexture();
    
    bool   isTextured() { return objectTextures.size() != 0; }
    void   toGPUTexture(GLuint program);

    virtual void aplicaTG(glm::mat4 m) {
      modelMatrix = m;
    }

protected:

    shared_ptr<GPUMaterial> material;   // Material de l'objecte

    std::vector <glm::vec4> objectVertices;
    std::vector <glm::vec4> objectColors;
    std::vector <glm::vec4> objectNormals;
    std::vector <glm::vec2> objectTextures;

    GLuint  VAO;
    GLuint  vertex_buffer, texture_buffer, color_buffer, normal_buffer;

    GLuint textureID;
    
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    
};

