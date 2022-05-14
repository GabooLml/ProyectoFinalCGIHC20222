#pragma once

#include <glew.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>

#include <glfw3.h>

#include "Keyframe.h";

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, glm::vec3* refPosicion, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed, bool* ptrCamaraPersonaje);

	void keyControl(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);

	glm::vec3 getCameraPosition(){ return position; }
	void setCameraPosition(glm::vec3* newPosition);
	void setAnimacionPtr(Keyframe* ptrAnimacion) { animacionHelicoptero = ptrAnimacion; }
	void positionControl(bool* banderaAnimacion, GLfloat deltaTime, GLfloat movX, GLfloat movY, GLfloat movZ);
	void rotationControl(bool* banderaAnimacion, GLfloat deltaTime, GLfloat yaw, GLfloat pitch );
	void addYawPitch(GLfloat newYaw, GLfloat newPitch) { yaw += newYaw; pitch += newPitch; update(); }
	void setYawPitch(GLfloat newYaw, GLfloat newPitch) { yaw = newYaw; pitch = newPitch; update(); }
	glm::vec3 getCameraDirection();
	glm::mat4 calculateViewMatrix();
	//Solo se utilizan 
	GLfloat getYaw() { return yaw; }
	GLfloat getPitch() { return pitch; }
	glm::vec3 getFront() { return front; }
	glm::vec3 getRight() { return right; }
	glm::vec3 geDirecionVector();

	~Camera();

private:
	bool* camaraPersonaje;
	glm::vec3* posModelo;
	glm::vec3 position;
	//Una variable que distanciará la cámara dependiendo del frente.
	glm::vec3 modelOfsett;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	Keyframe* animacionHelicoptero;

	GLfloat yaw;
	GLfloat pitch;

	GLfloat moveSpeed;
	GLfloat turnSpeed;

	void update();
};

