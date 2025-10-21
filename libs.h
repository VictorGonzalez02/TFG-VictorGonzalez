#pragma once

#include<iostream>
#include<cstdlib>
#include<fstream>
#include<string>
#include<vector>
#include <ctime>

#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/vec2.hpp>
#include<glm/vec3.hpp>
#include<glm/vec4.hpp>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include"external/imgui/imgui.h"
#include"external/imgui/backends/imgui_impl_glfw.h"
#include"external/imgui/backends/imgui_impl_opengl3.h"
#include"tinyfiledialogs.h"

#include<soil2/SOIL2.h>

#include"Utilities/Shader.h";
#include"GeometricObjects/Material.h";
#include"GeometricObjects/Vertex.h"
#include"GeometricObjects/Mesh.h"
#include"GeometricObjects/Primitives.h"
#include"Light.h"
#include"enums.h"

