#pragma once


#include<iostream>

#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>

#include<glm/vec3.hpp>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>

enum direction {FORWARD = 0, BACKWARD, LEFT, RIGHT, UP, DOWN};

class Camera
{
private:
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	GLfloat movementSpeed;
	GLfloat sensistivity;

	glm::vec3 worldUp;
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	GLfloat pitch;
	GLfloat yaw;
	GLfloat roll;

	int framebufferWidth;
	int framebufferHeight;

	float nearPlane;
	float farPlane;
	float fov;

	bool modeFree;
	float cameraDistanceFromCenter = 80.0f; 
	glm::vec3 centerOfPointCloud = glm::vec3(0.0f); 

	void updateCameraVectors()
	{
		if (modeFree) {
			
			
			// Create a 4x4 matrix from the quaternion// The initial direction of the front vector
			//glm::vec3 initialFront = glm::vec3(0.0f, 0.0f, -1.0f);

			//// Apply the rotation to the initial front vector
			//glm::quat frontQuat = this->getOrientation() * glm::quat(0, initialFront) * glm::inverse(this->getOrientation());
			//glm::vec3 newFront = glm::normalize(glm::vec3(frontQuat.x, frontQuat.y, frontQuat.z));
			
			glm::vec3 newFront;
			newFront.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
			newFront.y = sin(glm::radians(this->pitch));
			newFront.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
			newFront = glm::normalize(newFront);

			//// Lerp between old front and new front
			float t = 0.1f; // This value can be changed to make the transition faster or slower
			this->front = (1 - t) * this->front + t * newFront;
		}
		else {
			
			/*glm::vec3 initialVector = glm::vec3(0.0f, 0.0f, -cameraDistanceFromCenter);
			glm::quat newPositionQuat = this->getOrientation() * glm::quat(0, initialVector) * glm::inverse(this->getOrientation());
			glm::vec3 newPosition = glm::vec3(newPositionQuat.x, newPositionQuat.y, newPositionQuat.z);
			newPosition += centerOfPointCloud;*/
			vec3 newPosition;
			newPosition.x = centerOfPointCloud.x + cameraDistanceFromCenter * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
			newPosition.y = centerOfPointCloud.y + cameraDistanceFromCenter * sin(glm::radians(pitch));
			newPosition.z = centerOfPointCloud.z + cameraDistanceFromCenter * cos(glm::radians(pitch)) * sin(glm::radians(yaw));

			// Lerp between old position and new position
			float t = 0.1f; // This value can be changed to make the transition faster or slower
			position = (1 - t) * position + t * newPosition;

			this->front = glm::normalize(centerOfPointCloud - position);
		}
		
		
		this->right = glm::normalize(glm::cross(this->front, this->worldUp));
		this->up = glm::normalize(glm::cross(this->right, this->front));
	};

public:
	Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 worldUp, int frameBufferWidth, int frameBufferHeight)
	{
		this->ViewMatrix = glm::mat4(1.f);

		this->movementSpeed = 10.f;
		this->sensistivity = 56.f;

		this->worldUp = worldUp;
		this->position = position;

		// Normaliza la direcci�n para evitar errores en los c�lculos
		direction = glm::normalize(direction);
		
		this->right = glm::normalize(glm::cross(up, direction));
		this->up = glm::cross(direction, this->right);

		// Calcula los valores iniciales de pitch y yaw basados en la direcci�n dada
		this->pitch = glm::degrees(asin(direction.y));
		this->yaw = glm::degrees(atan2(direction.z, direction.x));

		this->roll = 0.f;

		this->fov = 90.f;
		this->nearPlane = 0.1f;
		this->farPlane = 1000.f;
		this->framebufferHeight = frameBufferHeight;
		this->framebufferWidth = frameBufferWidth;

		this->updateCameraVectors();
	};

	~Camera(){}

	//Accessors
	const glm::mat4 getViewMatrix()
	{
		//this->ViewMatrix = glm::lookAt(this->position, this->position + this->front, this->up);
		if (modeFree) {
			this->ViewMatrix = glm::lookAt(this->position, this->position + this->front, this->up);
		}
		else {
			this->ViewMatrix = glm::lookAt(this->position, this->front, this->up);
		}

		this->updateCameraVectors();

		return this->ViewMatrix;
	}

	const glm::mat4 getProjectionMatrix()
	{
		this->ProjectionMatrix = glm::perspective(
			glm::radians(this->fov),
			static_cast<float>(this->framebufferWidth) / this->framebufferHeight,
			this->nearPlane,
			this->farPlane
		);

		return this->ProjectionMatrix;
	}

	void toggleCameraMode() {
		modeFree = !modeFree;
	}


	const glm::vec3 getPosition() const
	{
		return this->position;
	}

	void setPosition(glm::vec3 position) 
	{
		this->position = position;
	}

	const glm::vec3 getFront() const
	{
		return this->front;
	}

	const GLfloat getPitch() const
	{
		return this->pitch;
	}

	const GLfloat getYaw() const
	{
		return this->yaw;
	}

	const GLfloat getRoll() const
	{
		return this->roll;
	}

	bool* getFreeMode() {
		return &this->modeFree;
	}

	float getNearPlane() {
		return this->nearPlane;
	}

	float getFarPlane() {
		return this->farPlane;
	}

	float getFov() {
		return this->fov;
	}

	//Functions
	void move(const float& dt, const int direction)
	{
		//Update position vector
		if (this->modeFree) {
			switch (direction)
			{
			case FORWARD:
				this->position += this->front * this->movementSpeed * dt;
				break;
			case BACKWARD:
				this->position -= this->front * this->movementSpeed * dt;
				break;
			case LEFT:
				this->position -= this->right * this->movementSpeed * dt;
				break;
			case RIGHT:
				this->position += this->right * this->movementSpeed * dt;
				break;
			case UP:
				this->position -= this->up * this->movementSpeed * dt;
				break;
			case DOWN:
				this->position += this->up * this->movementSpeed * dt;
				break;
			default:
				break;
			}
		}
		else {
			switch (direction)
			{
			case FORWARD:
				this->cameraDistanceFromCenter -= this->movementSpeed * dt;
				break;
			case BACKWARD:
				this->cameraDistanceFromCenter += this->movementSpeed * dt;
				break;
			default:
				
				break;
			}
		}

		
	}

	void updateMouseInput(const float& dt, const double& offsetX, const double& offsetY, bool mousePressed)
	{
		/*if (modeFree || mousePressed)
		{
			yaw += static_cast<GLfloat>(offsetX) * this->sensistivity * dt;
			pitch += static_cast<GLfloat>(offsetY) * this->sensistivity * dt;
		}*/
		
		//Update pitch yaw and roll
		if (this->modeFree || mousePressed) {
			yaw += static_cast<GLfloat>(offsetX) * this->sensistivity * dt;
			pitch += static_cast<GLfloat>(offsetY) * this->sensistivity * dt;
			pitch = glm::clamp(pitch, -89.0f, 89.0f);
			if (this->yaw > 360.f || this->yaw < -360.f)
				this->yaw = 0;
		}
	}

	glm::quat getOrientation() {
		float orient = 1;
		if (this->modeFree)
			orient = -1;
		glm::quat q = glm::angleAxis(glm::radians(yaw), glm::vec3(0, orient, 0));
		q *= glm::angleAxis(glm::radians(-pitch), glm::vec3(-1, 0, 0)) ;

		return glm::normalize(q);
	}
	void updateInput(const float& dt, const int direction, const double& offsetX, const double& offsetY, bool mousePressed)
	{
		this->updateMouseInput(dt, offsetX, offsetY, mousePressed);
	}

	void updateFramebufferSize(int framebufferWidth, int framebufferHeight) {
		this->framebufferWidth = framebufferWidth;
		this->framebufferHeight = framebufferHeight;
	}

	void setNearPlane(float nearPlane)
	{
		this->nearPlane = nearPlane;
	}

	void setFarPlane(float farPlane)
	{
		this->farPlane = farPlane;
	}

	void setCenterPointCloud(glm::vec3 centerOfPointCloud) {
		this->centerOfPointCloud = centerOfPointCloud;
	}

	void setFov(float scroll)
	{
		// make sure field of view stays within reasonable bounds
		if (this->fov - scroll < 1.0f) {
			this->fov = 1.0f;
		}
		else if (this->fov - scroll > 90.0f) {
			this->fov = 90.0f;
		}
		else {
			this->fov -= scroll; // adjust field of view based on mouse wheel

		}
		
	}

};
