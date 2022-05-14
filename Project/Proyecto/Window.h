#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>
#include "SpotLight.h"
#include "CommonValues.h"

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);
	}
	GLint getLight() { return lightCount; }
	GLint getLightLamp() { return lightHelicopterOff; }
	GLfloat getCarLap() { return carlap; }
	GLfloat getHelicopterX() { return helicopterX; }
	GLfloat getHelicopterY() { return helicopterY; }
	GLfloat getHelicopterZ() { return helicopterZ; }
	GLfloat getHelicopterLap() { return helicopterlap; }
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }

	void linkBools(bool* camaraPersonajePTR, bool* animacionHelicopteroPTR) {
		camaraPersonaje = camaraPersonajePTR;
		animacionHelicoptero = animacionHelicopteroPTR;
	};
	

	unsigned int* counter;
	SpotLight* arrayPointer;

	~Window();
private:
	GLFWwindow* mainWindow;
	GLint width, height;
	//SpotLight spotLigths[MAX_SPOT_LIGHTS];
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	//Para manejar banderas
	bool* camaraPersonaje;
	bool* animacionHelicoptero;
	//Para manejo de camaras
	GLfloat offsetZ = 0.0f;
	GLfloat offsetX = 0.0f;
	//para manejar las spotlights
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	GLint lightCount;
	GLint lightHelicopterOff;
	GLfloat carlap;
	GLfloat helicopterX;
	GLfloat helicopterY;
	GLfloat helicopterZ;
	GLfloat helicopterlap;
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};