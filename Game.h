#pragma once

#include "libs.h";
#include "Camera.h";



class Game
{
private:

	//VARIABLES
	GLFWwindow* window;
	const int WINDOW_WIDTH;
	const int WINDOW_HEIGHT;
	int framebufferWidth;
	int framebufferHeight;

	//Opengl Context
	const int GL_VERSION_MAJOR;
	const int GL_VERSION_MINOR;

	int currentShader;
	int currentPointCloud;
	int octreeError;
	int octreeGround;
	int shadowMode;
	char fileURL[128];

	//Delta time
	float dt;
	float curTime;
	float lastTime;

	//Mouse input
	double lastMouseX;
	double lastMouseY;
	double mouseX;
	double mouseY;
	double mouseOffsetX;
	double mouseOffsetY;
	bool firstMouse;
	float u_mouseX;
	float u_mouseY;


	//Camera
	Camera camera;
	bool cameraModeFree;

	//Matrices
	glm::mat4 ViewMatrix;
	glm::vec3 camPosition;
	glm::vec3 worldUp;
	glm::vec3 camFront;
	glm::mat4 ProjectionMatrix;

	float fov;
	float nearPlane;
	float farPlane;

	//Shaders
	std::vector<Shader*> shaders;

	//PointClouds
	std::vector<PointCloud*> pointClouds;
	
	//Materials
	std::vector<Material*> materials;

	//viewPort
	Mesh* viewPort;

	//Lights
	std::vector <PointLight*> pointLights;

	//Global light vector
	glm::vec3 globalLight;

	//Epsilon
	float uEpsilon;
	float uMaxRayDistance;

	//randomSeed
	glm::vec2 randomSeed;

	//
	int mode = 1;
	bool octreeModeSelected;
	bool voxelModeSelected;
	bool m_fileDialogOpen;
	//ImFileDialogInfo m_fileDialogInfo;


	//STATIC VARIABLES
	

	//PRIVATE FUNCTIONS
	void initGLFW();
	void initWindow(const char* title, bool resizable);
	void initGLEW(); //AFTER CONTEXT CREATION!!!
	void initOpenGLOptions();
	void initMatrices();
	void initShaders();
	void initViewPort();
	void initPointCloud();
	void initMaterials();
	void initPointLights();
	void initLights();
	void initUniforms();
	void initImGui();

	void renderImGui();
	void updateUniforms();
	void updateRandom();



public:

	//CONSTRUCTORS / DESTRUCTORS

	Game(const char* title,
		const int WINDOW_WIDTH, const int WINDOW_HEIGHT,
		const int GL_VERSION_MAJOR, const int GL_VERSION_MINOR,
		bool resizable);

	virtual ~Game();

	//ACCESSORS
	int getWindowShouldClose();

	//MODIFIERS
	void setWindowShouldClose();


	//FUNCTIONS
	void updateDt();
	void updateKeyboardInput();
	void updateMouseInput();
	void updateInput();
	void updateMeshPosition();
	void update();
	void render();

	//STATIC FUNCTIONS
	static void framebuffer_resize_callback(GLFWwindow* window, int fbW, int fbH);
	/*static void updateInput(GLFWwindow* window);
	static void updateInput(GLFWwindow* window, Mesh& mesh);*/


};

