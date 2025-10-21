#include "Game.h"



//Private functions
void Game::initGLFW()
{
	//Init glfw
	if (glfwInit() == GLFW_FALSE)
	{
		std::cout << "ERROR::GLFW_INIT_FAILED" << "\n";
		glfwTerminate();
	}
}

void Game::initWindow(
	const char* title,
	bool resizable
)
{
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//FOR THE VERSION X.Y MAJOR = X, MINOR = Y
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, this->GL_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, this->GL_VERSION_MINOR);
	glfwWindowHint(GLFW_RESIZABLE, resizable);

	this->window = glfwCreateWindow(this->WINDOW_WIDTH, this->WINDOW_HEIGHT, title, NULL, NULL);

	if (this->window == nullptr) 
	{
		std::cout << "ERROR::GLFW_WINDOW_INIT_FAILED" << "\n";
		glfwTerminate();
	}

	//Solo lo necesitamos si no queremos hacer el resize
	glfwGetFramebufferSize(this->window, &this->framebufferWidth, &this->framebufferHeight);
	//glViewport(0, 0, framebufferWidth, framebufferHeight);
	//En case de resize
	glfwSetFramebufferSizeCallback(this->window, framebuffer_resize_callback);

	glfwMakeContextCurrent(this->window); //IMPORTANT!!

}

void Game::initGLEW()
{
	//INIT GLEW (NEEDS WINDOW AND OOPENGL CONTEXT)
	//GLEW make what functions are available and right can use it.
	glewExperimental = GL_TRUE;

	//check error
	if (glewInit() != GLEW_OK)
	{
		std::cout << "ERROR::GLEW_INIT_FAILED" << "\n";
		glfwTerminate();
	}
}

void Game::initOpenGLOptions()
{
	//opengl functions les he de mirar segons em vagin be a mi.
	//open gl is a state machine, anything we enable would be enable.

	//Makes possible z coordinate
	glEnable(GL_DEPTH_TEST);

	//texture3d
	//glEnable(GL_TEXTURE_3D);
	//PFNGLTEXIMAGE3DPROC glTexImage3D;
	//glTexImage3D = (PFNGLTEXIMAGE3DPROC) glfwGetProcAddress("glTexImage3D");

	//
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	//Most important, blending of colors. If you want to enable settings for this blend, the next
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //GL_LINE TODO EN LINEAS, GL_FILL TODO RELLENO, ETC.

	//Input cursor
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Game::initMatrices()
{

	this->ViewMatrix = glm::mat4(1.f);
	this->ViewMatrix = glm::lookAt(this->camPosition, this->camPosition + this->camFront, this->worldUp);

	//Dependemos del framebuffer.
	this->ProjectionMatrix = glm::perspective(
		glm::radians(this->fov),
		static_cast<float>(this->framebufferWidth) / this->framebufferHeight,
		this->nearPlane,
		this->farPlane
	);
}

void Game::initShaders()
{
	//SHADER INIT
	this->shaders.push_back(
		new Shader(
			this->GL_VERSION_MAJOR,
			this->GL_VERSION_MINOR,
			"vertex_core.glsl",
			"fragment_core_Octree_shader_multi.glsl"));
	this->shaders.push_back(
		new Shader(
		this->GL_VERSION_MAJOR, 
		this->GL_VERSION_MINOR, 
			"vertex_core.glsl", 
			"fragment_core_voxel_DDA.glsl"));
	
	
}

void Game::initPointCloud()
{
	this->pointClouds.push_back(new PointCloud(1, "OBJFiles/1_0_0.ply"));//capsule.obj"));
	//this->camera.setPosition(this->pointClouds[0]->GetMaxVertex() + 5.f);
}

void Game::initMaterials()
{
	this->materials.push_back(new Material(
		glm::vec3(1.f), glm::vec3(1.f,0.f,0.f), glm::vec3(1.f),
		1.f,
		1.f,
		0, 
		1));
}

void Game::initViewPort()
{
	
	
	this->viewPort = new Mesh(
		new Quad(),
		this->materials[MAT_0], //Mandar 3 materiales distintos.
		this->pointClouds[0],
		this->camera.getPosition() - this->camera.getFront() * 2.f,
		glm::vec3(0.f),
		glm::vec3(0.f),
		glm::vec3(1.f)
	);

	

	
}


void Game::initPointLights()
{
	this->pointLights.push_back(new PointLight(
		glm::vec3(10.f,10.f,10.f), 
		0.5f,
		glm::vec3(1.f, 1.f, 1.f)
	));
}

void Game::initLights()
{
	this->globalLight = glm::vec3(0.1, 0.1, 0.1);
	this->initPointLights();
}

void Game::updateRandom()
{
	srand(time(nullptr));

	// Genera dos números aleatorios entre 0 y 1
	float random1 = static_cast<float>(rand()) / RAND_MAX;
	float random2 = static_cast<float>(rand()) / RAND_MAX;

	this->randomSeed = glm::vec2(random1, random2);
}

void Game::initUniforms()
{
	this->uEpsilon = 0.0001;
	this->uMaxRayDistance = 999.9f;
	this->updateRandom();
	this->shaders[this->currentShader]->setVec2f(this->randomSeed, "randomSeed");
	this->shaders[this->currentShader]->set1f(this->uEpsilon, "uEpsilon");
	this->shaders[this->currentShader]->set1f(this->uMaxRayDistance, "uMaxRayDistance");
	this->pointClouds[this->currentPointCloud]->initPlanes(this->shaders[this->currentShader]);

	this->shaders[this->currentShader]->setVec3f(this->globalLight, "globalLight");
	this->shaders[this->currentShader]->setMat4fv(this->ViewMatrix, "ViewMatrix");
	this->shaders[this->currentShader]->setMat4fv(this->ProjectionMatrix, "ProjectionMatrix");
	this->shaders[this->currentShader]->setVec2f(glm::vec2(this->framebufferWidth,this->framebufferHeight), "window_size");

	for(PointLight* pl : this->pointLights)
	{
		pl->sendToShader(*this->shaders[this->currentShader]);
	}
}

void Game::initImGui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	this->octreeModeSelected = true;
	this->voxelModeSelected = false;
}


void Game::renderImGui()
{
	// ImGui NewFrame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Create ImGui windows
	
	static int maxPoints = 0;
	static int maxDepth = 0;
	static bool segmented = false;
	static bool error = false;
	static bool shadow = false;

	if (ImGui::Begin("Configuración")) {
		// Add widgets to the empty window here...
		ImGui::InputText("File URL", fileURL, IM_ARRAYSIZE(fileURL));

		if (ImGui::Button("Seleccionar archivo")) {
			const char* filterPatterns[1] = { "*.ply" };
			const char* fileName = tinyfd_openFileDialog(
				"Selecciona un archivo",  // Título de la ventana
				"",  // Ruta inicial (vacía para usar la ruta por defecto)
				1,  // Número de patrones de filtro
				filterPatterns,  // Patrones de filtro
				"Image Files",  // Descripción de los patrones de filtro
				0  // 0 para permitir múltiple selección, 1 para permitir solo una selección
			);

			if (fileName != NULL) {
				strncpy_s(fileURL, fileName, sizeof(fileURL));
				fileURL[sizeof(fileURL) - 1] = '\0';  // Asegurar que la cadena termine con '\0'
			}
		}

		ImGui::Checkbox("FreeMode", this->camera.getFreeMode());



		if (ImGui::Checkbox("Voxel", &voxelModeSelected)) {
			if (voxelModeSelected) octreeModeSelected = false;
			this->currentShader = SHADER_VOXEL;
			this->initUniforms();
		}

		if (ImGui::Checkbox("Octree", &octreeModeSelected)) {
			if (octreeModeSelected) voxelModeSelected = false;
			this->currentShader = SHADER_OCTREE;
			this->initUniforms();
		}
		if (octreeModeSelected) {
			ImGui::InputInt("Máximo de puntos", &maxPoints);
			ImGui::InputInt("Máximo de profundidad", &maxDepth);
			ImGui::Checkbox("Segmented", &segmented);
			ImGui::Checkbox("Error", &error);
			ImGui::Checkbox("Shadow", &shadow);
			if (error) {
				octreeError = 0;
			}
			else {
				octreeError = 1;
			}
			if (segmented) {
				octreeGround = 0;
			}
			else {
				octreeGround = 1;
			}
			if (shadow) {
				shadowMode = 1;
			}
			else {
				shadowMode = 0;
			}
		}
		if (ImGui::Button("Cargar")) {
			//cargarNuevoPointCloud();
			this->pointClouds.push_back(new PointCloud(1, this->fileURL));
			this->currentPointCloud = this->pointClouds.size() - 1;
			this->camera.setCenterPointCloud(this->pointClouds[this->currentPointCloud]->getCenter());
			this->viewPort->setPointCloud(this->pointClouds[this->currentPointCloud]);
			this->initUniforms();
		}

		if (octreeModeSelected) {
			ImGui::SliderFloat("Radius", this->pointClouds[this->currentPointCloud]->getRadius(), 0.01f, 3.0);
			ImGui::SliderFloat("Alpha ground", this->pointClouds[this->currentPointCloud]->getAlphaGround(), 0.01f, 1.0);
			ImGui::SliderFloat("Alpha error", this->pointClouds[this->currentPointCloud]->getAlphaError(), 0.01f, 1.0);
		}



		ImGui::ColorEdit3("Global Light", reinterpret_cast<float*>(&this->globalLight.x));


		ImGui::ColorEdit3("Ambient", reinterpret_cast<float*>(&materials[0]->getAmbient()->x));
		ImGui::ColorEdit3("Diffuse", reinterpret_cast<float*>(&materials[0]->getDiffuse()->x));
		ImGui::ColorEdit3("Specular", reinterpret_cast<float*>(&materials[0]->getSpecular()->x));

		
		ImGui::ColorEdit3("Ia", reinterpret_cast<float*>(&pointLights[0]->getIa()->x));
		ImGui::ColorEdit3("Id", reinterpret_cast<float*>(&pointLights[0]->getId()->x));
		ImGui::ColorEdit3("Is", reinterpret_cast<float*>(&pointLights[0]->getIs()->x));

		ImGui::SliderFloat("Intensity", pointLights[0]->getIntensity(), 0.0f, 1.0f);
		ImGui::SliderFloat("Constant", pointLights[0]->getConstant(), 0.0f, 1.0f);
		ImGui::SliderFloat("Linear", pointLights[0]->getLinear(), 0.0f, 1.0f);
		ImGui::SliderFloat("Quadratic", pointLights[0]->getQuadratic(), 0.0f, 1.0f);

		ImGuiIO& io = ImGui::GetIO();
		float fps = io.Framerate;
		ImGui::Text("FPS: %.1f", fps);

	}


	

	ImGui::End();
	ImGui::Render();
}



void Game::updateUniforms()
{
	this->shaders[this->currentShader]->set1i(this->mode, "mode");
	this->shaders[this->currentShader]->set1i(this->octreeError, "octreeError");
	this->shaders[this->currentShader]->set1i(this->octreeGround, "octreeGround");
	this->shaders[this->currentShader]->set1i(this->shadowMode, "shadowMode");

	//this->shaders[this->currentShader]->set1f(this->u_mouseX, "u_mouseX"); no se sabe es para hace rel raytracing de un punto.
	//this->shaders[this->currentShader]->set1f(this->u_mouseY, "u_mouseY");
	
	this->updateRandom();
	this->shaders[this->currentShader]->setVec2f(this->randomSeed, "randomSeed");

	this->shaders[this->currentShader]->set1i(7, "texture3DD");
	//this->shaders[this->currentShader]->set1i(1, "u_nodesTextureFloat");
	//this->shaders[this->currentShader]->set1i(2, "u_nodesTextureInt");
	//this->shaders[this->currentShader]->set1i(3, "u_pointsTexture");

	//this->shaders[this->currentShader]->set1i(4, "u_nodesTextureFloatError");
	//this->shaders[this->currentShader]->set1i(5, "u_nodesTextureIntError");
	//this->shaders[this->currentShader]->set1i(6, "u_pointsTextureError");
	
	this->shaders[this->currentShader]->set1i(8, "u_multiNodesTextureFloat");
	this->shaders[this->currentShader]->set1i(9, "u_multiNodesTextureInt");
	this->shaders[this->currentShader]->set1i(10, "u_multiPointsTexture1");
	this->shaders[this->currentShader]->set1i(11, "u_multiPointsTexture2");

	this->shaders[this->currentShader]->setVec3f(this->globalLight, "globalLight");

	//Update view matrix
	this->ViewMatrix = this->camera.getViewMatrix();
	this->shaders[this->currentShader]->setMat4fv(inverse(this->ViewMatrix), "ViewMatrix");
	this->shaders[this->currentShader]->setVec3f(this->camera.getPosition(), "camPosition");

	//Send lights
	for (PointLight* pl : this->pointLights)
	{
		pl->sendToShader(*this->shaders[this->currentShader]);
	}


	//Update Projection Matrix
	glfwGetFramebufferSize(this->window, &this->framebufferWidth, &this->framebufferHeight);
	this->shaders[this->currentShader]->setVec2f(glm::vec2(this->framebufferWidth, this->framebufferHeight), "window_size");

	this->camera.updateFramebufferSize(this->framebufferWidth, this->framebufferHeight);
	this->ProjectionMatrix = this->camera.getProjectionMatrix();

	this->shaders[this->currentShader]->setMat4fv(inverse(this->ProjectionMatrix), "ProjectionMatrix");

}



Game::Game(const char* title,
	const int WINDOW_WIDTH, const int WINDOW_HEIGHT,
	const int GL_VERSION_MAJOR, const int GL_VERSION_MINOR,
	bool resizable) 
	: 
	WINDOW_WIDTH(WINDOW_WIDTH),
	WINDOW_HEIGHT(WINDOW_HEIGHT),
	GL_VERSION_MAJOR(GL_VERSION_MAJOR), 
	GL_VERSION_MINOR(GL_VERSION_MINOR),
	camera(glm::vec3(20.f, 20.f, 20.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f, 1.f, 0.f), WINDOW_WIDTH, WINDOW_HEIGHT)
{
	//Init variables
	this->window = nullptr;
	this->framebufferWidth = this->WINDOW_WIDTH;
	this->framebufferHeight = this->WINDOW_HEIGHT;

	
	//Basic camera ViewMatrix
	this->camPosition = glm::vec3(0.f, 0.f, 1.f);
	this->worldUp = glm::vec3(0.f, 1.f, 0.f);
	this->camFront = glm::vec3(0.f, 0.f, -1.f);

	//ProjectionMatrix
	this->fov = 90.f;
	this->nearPlane = 0.1f;
	this->farPlane = 1000.f;
	this->ProjectionMatrix = glm::mat4(1.f);

	//Input mouse
	this->dt = 0.f;
	this->curTime = 0.f;
	this->lastTime = 0.f;

	this->lastMouseX = 0.0;
	this->lastMouseY = 0.0;
	this->mouseX = 0.0;
	this->mouseY = 0.0;
	this->mouseOffsetX = 0.0;
	this->mouseOffsetY = 0.0;
	this->firstMouse = true;

	this->currentShader = SHADER_OCTREE;
	this->currentPointCloud = 0;

	this->octreeError = 1;
	this->octreeGround = 1;

	this->initGLFW(); //OK
	this->initWindow(title, resizable); //OK
	this->initGLEW(); //OK
	this->initImGui(); //OK
	this->initOpenGLOptions(); //OK
	this->initMatrices(); //OK

	this->initShaders(); //OK
	this->initPointCloud(); //OK
	this->camera.setCenterPointCloud(this->pointClouds[0]->getCenter());
	this->initMaterials(); //OK
	this->initViewPort(); 
	this->initLights();
	this->initUniforms();

}


Game::~Game()
{
	glfwDestroyWindow(this->window);
	glfwTerminate();

	for (size_t i = 0; i < this->shaders.size(); i++)
		delete this->shaders[i];

	for (size_t i = 0; i < this->materials.size(); i++)
		delete this->materials[i];

	for (size_t i = 0; i < this->pointClouds.size(); i++)
		delete this->pointClouds[i];

	
	delete this->viewPort;

	for (size_t i = 0; i < this->pointLights.size(); i++)
		delete this->pointLights[i];

	// ImGui Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

//Accessors
int Game::getWindowShouldClose()
{
	return glfwWindowShouldClose(this->window);
}

//Modifiers
void Game::setWindowShouldClose()
{
	glfwSetWindowShouldClose(this->window, GLFW_TRUE);
}


//Functions
void Game::updateDt()
{
	this->curTime = static_cast<float>(glfwGetTime());
	this->dt = this->curTime - this->lastTime;
	this->lastTime = this->curTime;
}

void Game::updateMouseInput()
{
	glfwGetCursorPos(this->window, &this->mouseX, &this->mouseY);

	if (this->firstMouse)
	{
		this->lastMouseX = this->mouseX;
		this->lastMouseY = this->mouseY;
		this->firstMouse = false;
	}

	//Calc offset
	this->mouseOffsetX = this->mouseX - this->lastMouseX;
	this->mouseOffsetY = this->lastMouseY - this->mouseY;


	//Set last X and Y
	this->lastMouseX = this->mouseX;
	this->lastMouseY = this->mouseY;

	//Move light
	if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		//this->pointLights[0]->setPosition(this->camera.getPosition());
		this->pointLights[0]->setPosition(this->camera.getPosition());
	}
	if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		//this->u_mouseX = (2.0f * mouseX) / this->WINDOW_WIDTH - 1.0f; TODO PARA EL RAYTREACING DE UN SOLO PUNTO
		//this->u_mouseY - (2.0f * mouseY) / this->WINDOW_HEIGHT; 
	}

	

	
}



void Game::updateKeyboardInput()
{
	//Program
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		setWindowShouldClose();
	}

	//Movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		this->camera.move(this->dt, FORWARD);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		this->camera.move(this->dt, BACKWARD);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		this->camera.move(this->dt, LEFT);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		this->camera.move(this->dt, RIGHT);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		this->camera.move(this->dt, UP);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		this->camera.move(this->dt, DOWN);
	}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		this->pointClouds[this->currentPointCloud]->addRadius(this->dt);
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		this->pointClouds[this->currentPointCloud]->addRadius(-this->dt);
	}

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		this->pointClouds[this->currentPointCloud]->updatePlane(this->dt);
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
		this->pointClouds[this->currentPointCloud]->updatePlane(-this->dt);
	}

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		this->camera.setFov(0.05f);
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		this->camera.setFov(-0.05f);
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		this->mode = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		this->mode = 2;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		this->mode = 3;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		this->camera.toggleCameraMode();
	}
}

void Game::updateInput()
{
	glfwPollEvents();
	this->updateKeyboardInput();
	this->updateMouseInput();
	bool leftMousePressed = (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
	this->camera.updateInput(dt, -1, this->mouseOffsetX, this->mouseOffsetY, leftMousePressed);
}

void Game::updateMeshPosition() 
{

}

void Game::update()
{
	//Update Input
	this->updateDt();
	this->updateInput();
	this->updateUniforms();
}

void Game::render()
{
	//Update
	

	//DRAW
	//clear
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	

	renderImGui();

	
	

	//Render models
	this->viewPort->render(this->shaders[this->currentShader]);

	this->pointClouds[this->currentPointCloud]->render(this->shaders[this->currentShader]);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	//EndDraw
	glfwSwapBuffers(this->window);
	glFlush();

	//Resets
	glBindVertexArray(0);
	glUseProgram(0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//Static functions
void Game::framebuffer_resize_callback(GLFWwindow* window, int fbW, int fbH)
{
	glViewport(0, 0, fbW, fbH);
}
