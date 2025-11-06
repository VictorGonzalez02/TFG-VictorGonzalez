#include "GLWidget.hpp"

GLWidget::GLWidget(int w, int h) : world(nullptr), xRot(0.0f), yRot(0.0f), zRot(0.0f), xTra(0.0f), yTra(0.0f), program(0), shaderGL_Points(0),
mousePressed(false), lastMouseX(0.0), lastMouseY(0.0)
{
    // inicialització de la configuració
    config = GPUConfig(w, h); 
    transform = glm::mat4(1.0f);
    setupBuffer();
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
    activateShader("GL_Points", NULL);
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
    if(program->getId() == 3){
        world->draw();
    } else{
        computePass();
        displayPass();
    }
}

void GLWidget::initShadersGPU()
{
    shaderGL_Points = make_shared<GPUShader>("GL_Points", "vshader1.glsl", "fshader1.glsl");
    shaderZTest = make_shared<GPUShader>("ZTest", "cZTest.glsl");
    shaderZTestDisplay = make_shared<GPUShader>("ZTestDisplay", "vshader2.glsl", "fshader2.glsl");

    //this->shaders.push_back(new Shader(4, 5, "vertex_core.glsl", "fragment_core_Voxel_DDA.glsl"));

    // shaders per defecte
    program = shaderGL_Points;
    //programVoxel = shaders[0];
}

void GLWidget::activateShader(const char* typeShader, const char* nameTexture) {

    // TO DO: Modificar el mètode per a poder suportar més tipus de shaders
    if (std::strcmp(typeShader,"GL_Points")==0) {
        program = shaderGL_Points;
        program->use();
        world->toGPU(program->getId());
    } else if (std::strcmp(typeShader, "ZTest")==0){
        program = shaderZTest;
        program->use();
        world->toGPU(program->getId());
        setupBuffer();
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
    if(mousePressed){
        GLfloat yaw = 0.0f;
        GLfloat pitch = 0.0f;
        double dx = xpos - lastMouseX;
        double dy = ypos - lastMouseY;
        lastMouseX = xpos;
        lastMouseY = ypos;

        yaw += static_cast<GLfloat>(dx) * config.sensitivityAmount;
        pitch += static_cast<GLfloat>(dy) * config.sensitivityAmount;

        world->camera->updateCameraVectors(yaw, pitch);
        world->camera->toGPU(program->getId());
    }
    
    /*
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
    */
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

void GLWidget::loadPointCloud(const char* filename){
    PointCloud* p = new PointCloud(1, filename);
    world->addPointCloud(p);
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

void GLWidget::setupBuffer(){
    GLuint framebufferSSBO;
    // allocate
    glGenBuffers(1, &framebufferSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, framebufferSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, config.viewportWidth*config.viewportHeight*sizeof(uint32_t)*2, nullptr, GL_DYNAMIC_DRAW);

    // initialize: low (rgb) = 0 (black), high (depth) = 0xFFFFFFFF (far)
    std::vector<uint32_t> clear(config.viewportWidth*config.viewportHeight*2);
    for (size_t i = 0; i < config.viewportWidth*config.viewportHeight; ++i) {
        clear[i*2 + 0] = 0x000000u;      // rgb = black
        clear[i*2 + 1] = 0xFFFFFFFFu;   // depth = max (so any real closer depth will be smaller)
    }
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, clear.size()*sizeof(uint32_t), clear.data());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, framebufferSSBO);
}

void GLWidget::computePass(){
    //int numPoints = 983599;
    //int numPoints = 3609600;
    int numPoints = world->getNumPoints();
    glUniform1ui(glGetUniformLocation(program->getId(), "imageWidth"), config.viewportWidth);
    glUniform1ui(glGetUniformLocation(program->getId(), "imageHeight"), config.viewportHeight);
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "modelMatrix"),
                       1, GL_FALSE, glm::value_ptr(modelMatrix));

    uvec2 clearValue = uvec2(0x00000000u, 0xFFFFFFFFu); // "empty" = far depth
    glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT, &clearValue);
    glDispatchCompute((numPoints + 255)/256, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
}

void GLWidget::displayPass(){
    program = shaderZTestDisplay;
    program->use();

    GLuint fullscreenVAO, fullscreenVBO;
    {
        // Fullscreen quad (2 triangles covering [-1,1] range)
        const GLfloat fullscreenVertices[] = {
            //   X,    Y
            -1.0f, -1.0f,
            1.0f, -1.0f,
            1.0f,  1.0f,
            -1.0f, -1.0f,
            1.0f,  1.0f,
            -1.0f,  1.0f
        };

        glGenVertexArrays(1, &fullscreenVAO);
        glBindVertexArray(fullscreenVAO);

        glGenBuffers(1, &fullscreenVBO);
        glBindBuffer(GL_ARRAY_BUFFER, fullscreenVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(fullscreenVertices), fullscreenVertices, GL_STATIC_DRAW);

        // The fullscreen vertex shader expects layout(location = 0) in vec2 pos;
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

        glBindVertexArray(0);
    }

    glUniform1ui(glGetUniformLocation(program->getId(), "imageWidth"), config.viewportWidth);
    glBindVertexArray(fullscreenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2); // your fullscreen quad VAO
    program = shaderZTest;
    program->use();
}

void GLWidget::reset() {
    // Reset the scene
    world->scene->objects.clear();
    world->scene->pointCloud = nullptr;
    world->toGPU(program->getId());

}