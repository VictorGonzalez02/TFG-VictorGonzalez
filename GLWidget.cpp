#include "GLWidget.hpp"

GLWidget::GLWidget(int w, int h) : world(nullptr), xRot(0.0f), yRot(0.0f), zRot(0.0f), xTra(0.0f), yTra(0.0f), program(0), shaderColor(0),
mousePressed(false), lastMouseX(0.0), lastMouseY(0.0)
{
    // inicialització de la configuració
    config = GPUConfig(w, h); 
    transform = glm::mat4(1.0f);
}

GLWidget::~GLWidget()
{
}


void GLWidget::initializeGL()
{
    // inicialitzacions OpenGL
    setupOpenGLFeatures();
    
    // Inicialitzacions dels shaders
    std::cout << "Inicialització dels shaders\n";
    initShadersGPU();

    // Creació dels objectes de l'escena
    std::cout << "Inicialització del mon virtual\n";
    initWorld();  

    // Activació del shader per defecte i enviament del mon a la GPU
    activateShader("Color", NULL);
}

// Activa les característiques d'OpenGL que es faran servir
void GLWidget::setupOpenGLFeatures()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_RGBA);
    glEnable(GL_DOUBLE);
}

// Inicialització de la geometria de l'escena i preparació per enviar-la a la GPU
void GLWidget::initWorld()
{
    world = make_shared<GPUWorld>();
   
    auto lightsManager = make_shared<GPULightsManager>();
    world->setLightManager(lightsManager);

    auto camera = make_shared<GPUCamera>(config.observador, config.vrp, config.vup, config.fov, config.zNear, config.zFar,
                                        config.viewportWidth, config.viewportHeight);
    world->setCamera(camera);

    // Creació de l'escena
    auto scene = make_shared<GPUScene>();
    world->setScene(scene);
    world->setConfig(make_shared<GPUConfig>(config));
    world->updateAmbientLight(program->getId(), config.lightAmbientGlobal);

}


void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Dibuixar l'escena
    world->draw();
    //world->drawPointCloud();
}

void GLWidget::initShadersGPU()
{
    shaderColor = make_shared<GPUShader>("Color", "vshader1.glsl", "fshader1.glsl");
    //this->shaders.push_back(new Shader(4, 5, "vertex_core.glsl", "fragment_core_Voxel_DDA.glsl"));

    // shaders per defecte
    program = shaderColor;
    //programVoxel = shaders[0];
}

void GLWidget::activateShader(const char* typeShader, const char* nameTexture) {

    // TO DO: Modificar el mètode per a poder suportar més tipus de shaders
    if (std::strcmp(typeShader,"Color")==0) {
        program = shaderColor;
        program->use();
        world->toGPU(program->getId());
    } else if (std::strcmp(typeShader, "Voxel")==0){
        programVoxel = shaders[0];
        programVoxel->use();
        //world->toGPU(program->getId());
        world->toGPU_PointCloud(program->getId());
    } else {
        std::cerr << "Error: Tipus de shader desconegut." << std::endl;
    } 
    world->aplicaTG(transform);
}

// Funcions de control del mouse

void GLWidget::mousePressEvent(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mousePressed = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        } else if (action == GLFW_RELEASE) {
            mousePressed = false;
        }
    }
}

void GLWidget::mouseMoveEvent(GLFWwindow* window, double xpos, double ypos)
{
    if (mousePressed) {
        double dx = xpos - lastMouseX;
        double dy = ypos - lastMouseY;

        lastMouseX = xpos;
        lastMouseY = ypos;
       
        transform = glm::mat4(1.0f);
       
        // Update rotation angles based on mouse movement
        if (config.mouseMode ==1) {
            float newXRot = xRot + float(dy * config.sensitivityAmount);  // Reduced sensitivity for smoother rotation
            float newYRot = yRot + float(dx * config.sensitivityAmount);  // Reduced sensitivity for smoother rotation

            setXRotation(newXRot);
            setYRotation(newYRot);           
            
        } else if (config.mouseMode == 0) {
            xTra = xTra + float(0.1f * dx * config.sensitivityAmount); 
            yTra = yTra + float(0.1f * -dy * config.sensitivityAmount);   
            
        } 
        // TO DO - Implementar la translació amb el ratolí
        transform = glm::translate(transform, vec3(xTra, yTra, 0));
        transform = glm::rotate(transform, zRot, glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::rotate(transform, xRot, glm::vec3(1.0f, 0.0f, 0.0f));
        transform = glm::rotate(transform, yRot, glm::vec3(0.0f, 1.0f, 0.0f));
        
        // Enviar la matriu de transformació a la GPU
        world->setTGLastObject(transform);
    
    }
}

// Funcions per a la GUI

void GLWidget::setXRotation(float angle)
{
    // Normalize to [0, 360)
    angle = fmod(angle, 360.0f);
    if (angle < 0) angle += 360.0f;
    
    if (fabs(angle - xRot) > 0.01f) {
        xRot = angle;
    }
}

void GLWidget::setYRotation(float angle)
{
    // Normalize to [0, 360)
    angle = fmod(angle, 360.0f);
    if (angle < 0) angle += 360.0f;
    
    if (fabs(angle - yRot) > 0.01f) {
        yRot = angle;
    }
}

void GLWidget::setZRotation(float angle)
{
    // Normalize to [0, 360)
    angle = fmod(angle, 360.0f);
    if (angle < 0) angle += 360.0f;
    
    if (fabs(angle - zRot) > 0.01f) {
        zRot = angle;
    }
}

int GLWidget::getRenderMode() const {
    return config.renderMode;
}

void GLWidget::setRenderMode(int mode) {
    config.renderMode = mode;
}

void GLWidget::updateCamera() {
    world->updateCamera(program->getId(), config.observador, config.vrp, 
                             config.vup, config.fov, config.zNear, config.zFar);
}

void GLWidget::updateMaterial() {
    world->updateMaterial(program->getId(), config.ambientColor, config.diffuseColor, config.specularColor, config.shininess);
}

void GLWidget::updateGlobalAmbientLight() {
    world->updateAmbientLight(program->getId(), config.lightAmbientGlobal);
    
}

// Actualitza totes les llums, per què una ha estat activada o desactivada 
// o per què s'ha esborrat alguna de les llums
void GLWidget::updateAllLights() {
    world->updateAllLights(program->getId(), config.lights);
}

// Només han canviat alguna propietat de la llum que està a 'index' de la taula i està activa
void GLWidget::updateSingleLight(int index) {
    
    // Update this specific light in the shader
    world->updateSingleLight(program->getId(), config.lights[index], index);
       
}

void GLWidget::updateBackground() {

    // Color background
    glClearColor(config.backgroundColor.r, config.backgroundColor.g, config.backgroundColor.b, 1.0f);
}


// Other existing methods...

void GLWidget::loadObject(const char* filename) {
    // Load object from file
    // This function should load the object from the specified file
    // and update the object in the scene
    auto mesh = make_shared<Mesh>(filename);
    //mesh->make();
    
    // TO DO Fitxa 2: Cal afegir Material a l'objecte de forma aleatòria
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> colDist(0.0f, 1.0f); // Distribució per color
    glm::vec3 Ka(colDist(gen));
    glm::vec3 Kd(colDist(gen), colDist(gen), colDist(gen));
    glm::vec3 Ks(1.0f);
    shared_ptr<GPUMaterial> m = make_shared<GPUMaterial>(Ka, Kd, Ks, 100.0f);
    //mesh->setMaterial(m);

    //world->addObject(shared_ptr<Object>(mesh));
    //Quan afegim un objecte nou fem reset de la transformació actual, ja que volem una nova matriu
    xRot = 0;
    yRot = 0;
    zRot = 0;
    xTra = 0;
    yTra = 0;
    zTra = 0;

    // Cal actualitzar la GPU amb el nou objecte
    world->lastObjectToGPU(program->getId());
    world->aplicaTG(transform);
    
}

void GLWidget::addCube() {
    auto c = new Cub();
    c->make();

    // TO DO Fitxa 2: Cal afegir Material de forma aleatòria
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> colDist(0.0f, 1.0f); // Distribució per color
    glm::vec3 Ka(colDist(gen));
    glm::vec3 Kd(colDist(gen), colDist(gen), colDist(gen));
    glm::vec3 Ks(colDist(gen));
    shared_ptr<GPUMaterial> m = make_shared<GPUMaterial>(Ka, Kd, Ks, 100.0f);
    c->setMaterial(m);


    world->addObject(shared_ptr<Object>(c));
    //Quan afegim un objecte nou fem reset de la transformació actual, ja que volem una nova matriu
    xRot = 0;
    yRot = 0;
    zRot = 0;
    xTra = 0;
    yTra = 0;
    zTra = 0;

    // Cal actualitzar la GPU amb el nou objecte
    world->lastObjectToGPU(program->getId());
    world->aplicaTG(transform);
}

void GLWidget::reset() {
    // Reset the scene
    world->scene->objects.clear();
    world->toGPU(program->getId());

}