#pragma once

#include"libs.h";
#include"Camera.h";

class Light
{
protected:

	float intensity;
	
	glm::vec3 Ia;
	glm::vec3 Id;
	glm::vec3 Is;

public:
	/*
	 * Constructor de la classe Light.
	 * param Ia: component ambient de la llum.
	 * param Id: component difosa de la llum.
	 * param Is: component especular de la llum.
	 * */
	Light(float intensity, glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is)
	{
		this->intensity = intensity;
		this->Ia = Ia;
		this->Id = Id;
		this->Is = Is;
	}

	~Light()
	{

	}
	void setIa(const glm::vec3 Ia)
	{
		this->Ia = Ia;
	}

	void setId(const glm::vec3 Id)
	{
		this->Id = Id;
	}

	void setIs(const glm::vec3 Is)
	{
		this->Is = Is;
	}

	glm::vec3* getIa()
	{
		return &this->Ia;
	}

	glm::vec3* getId()
	{
		return &this->Id;
	}

	glm::vec3* getIs()
	{
		return &this->Is;
	}

	virtual void sendToShader(Shader& program) = 0;
};

class PointLight : public Light
{
protected:
	glm::vec3 position;
	float constant;
	float linear;
	float quadratic;

public:
	PointLight(glm::vec3 position, float intensity = 1.f, glm::vec3 Ia = glm::vec3(0.5f),
		glm::vec3 Id = glm::vec3(1.f), glm::vec3 Is = glm::vec3(0.5f),
		float constant = 1.f, float linear = 0.045f, float quadratic = 0.0075f)
		: Light(intensity, Ia, Id, Is)
	{
		this->position = position;
		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic;
	}

	~PointLight()
	{

	}

	void setPosition(const glm::vec3 position)
	{
		this->position = position;
	}

	void sendToShader(Shader& program) override
	{
		program.setVec3f(this->position, "pointlight.position");
		program.setVec3f(this->Ia, "pointlight.Ia");
		program.setVec3f(this->Id, "pointlight.Id");
		program.setVec3f(this->Is, "pointlight.Is");
		program.set1f(this->intensity, "pointlight.intensity");
		program.set1f(this->constant, "pointlight.constant");
		program.set1f(this->linear, "pointlight.linear");
		program.set1f(this->quadratic, "pointlight.quadratic");
	}	

	void setIntensity(const float intensity)
	{
		this->intensity = intensity;
	}

	void setConstant(const float constant)
	{
		this->constant = constant;
	}

	void setLinear(const float linear)
	{
		this->linear = linear;
	}

	void setQuadratic(const float quadratic)
	{
		this->quadratic = quadratic;
	}

	

	float* getIntensity()
	{
		return &this->intensity;
	}

	float* getConstant()
	{
		return &this->constant;
	}

	float* getLinear()
	{
		return &this->linear;
	}

	float* getQuadratic()
	{
		return &this->quadratic;
	}

	


};
