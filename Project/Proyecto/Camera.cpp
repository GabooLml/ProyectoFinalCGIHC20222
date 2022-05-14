#include "Camera.h"

Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, glm::vec3* refPosicion, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed, bool* ptrCamaraPersonaje)
{
	camaraPersonaje = ptrCamaraPersonaje;

	posModelo = refPosicion;
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime)
{
	GLfloat velocity = moveSpeed * deltaTime;
	glm::vec3 aux;
	if (keys[GLFW_KEY_W])
	{
		aux = glm::vec3(front.x, 0.0f, front.z);
		*posModelo += aux * velocity;
		position += aux * velocity;
	}

	if (keys[GLFW_KEY_S])
	{
		aux = glm::vec3(front.x, 0.0f, front.z);
		*posModelo -= aux * velocity;
		position -= aux * velocity;
	}

	if (keys[GLFW_KEY_A])
	{
		aux = glm::vec3(right.x, 0.0f, right.z);
		*posModelo -= aux * velocity;
		position -= aux * velocity;
	}

	if (keys[GLFW_KEY_D])
	{
		aux = glm::vec3(right.x, 0.0f, right.z);
		*posModelo += aux * velocity;
		position += aux * velocity;
	}
}

void Camera::positionControl(bool* banderaAnimacion, GLfloat deltaTime, GLfloat movX, GLfloat movY, GLfloat movZ ) {
	if (*banderaAnimacion) {
		GLfloat velocity;
		glm::vec3 aux;
		if (movX > 0.0f)
		{
			velocity = movX * deltaTime;
			aux = glm::vec3(front.x, 0.0f, front.z);
			*posModelo += aux * velocity;
			position += aux * velocity;
		}

		if (movX < 0.0f)
		{
			velocity = -movX * deltaTime;
			aux = glm::vec3(front.x, 0.0f, front.z);
			*posModelo -= aux * velocity;
			position -= aux * velocity;
		}

		if (movZ < 0.0f)
		{
			velocity = -movZ * deltaTime;
			aux = glm::vec3(right.x, 0.0f, right.z);
			*posModelo -= aux * velocity;
			position -= aux * velocity;
		}

		if (movZ > 0.0f)
		{
			velocity = movZ * deltaTime;
			aux = glm::vec3(right.x, 0.0f, right.z);
			*posModelo += aux * velocity;
			position += aux * velocity;
		}
		if (movY < 0.0f)
		{
			velocity = -movY * deltaTime;
			aux = glm::vec3(0.0f, up.y, 0.0f);
			*posModelo -= aux * velocity;
			position -= aux * velocity;
		}

		if (movY > 0.0f)
		{
			velocity = movY * deltaTime;
			aux = glm::vec3(0.0f, up.y, 0.0f);
			*posModelo += aux * velocity;
			position += aux * velocity;
		}
	}
}

void Camera::rotationControl(bool* banderaAnimacion, GLfloat deltaTime, GLfloat cambioX, GLfloat cambioY) {
	if (*banderaAnimacion) {
		cambioX *= turnSpeed;
		cambioY *= turnSpeed;

		yaw += cambioX;
		pitch += cambioY;



		if (pitch > 89.0f)
		{
			pitch = 89.0f;
		}

		if (pitch < -89.0f)
		{
			pitch = -89.0f;
		}

		update();
	}
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	update();
}

glm::mat4 Camera::calculateViewMatrix()
{
	if (*camaraPersonaje) {
		modelOfsett = glm::vec3(8.0f* front.x, 24.0f * front.y, 8.0f * front.z);
	}
	else {
		modelOfsett = glm::vec3(20.0f * front.x, 20.0f * front.y, 20.0f * front.z);
	}
	position = *posModelo - modelOfsett;
	return glm::lookAt(position, position + front, up);
}

glm::vec3 Camera::getCameraDirection()
{
	return glm::normalize(front);
}

glm::vec3 Camera::geDirecionVector() {
	return front;
}





void Camera::update()
{
	modelOfsett = glm::vec3(front.x, front.y, front.z);
	position = position - modelOfsett;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}

void Camera::setCameraPosition(glm::vec3 * newPosition) {
	position = * newPosition;
}

Camera::~Camera()
{
}
