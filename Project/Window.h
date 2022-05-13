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
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }

	void linkBools(bool* camaraPersonajePTR) {
		camaraPersonaje = camaraPersonajePTR;
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
	//para manejar las spotlights
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};