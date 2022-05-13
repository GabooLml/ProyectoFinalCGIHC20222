/*
Semestre 2022-2
Práctica 8: Iluminación 1
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//Para el manejo de archivos
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "Keyframe.h"
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera, cameraHeli;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;


Model Kitt_M;
Model Llanta_M;
Model Camino_M;
Model Blackhawk_M;
Model Dado_M;
Model Lampara_M;

Skybox madrugada, dia, atardecer, noche;


/// elementos para el acceso de archivos de animaciones

std::string filename;
std::ifstream file;
std::istringstream stream;
std::string line;
std::string word;
bool primerLinea = true;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

//Para el manejo de archivos
std::string nombresArchivos[] = {"ejemplo.txt" };
Keyframe animaciones[sizeof(nombresArchivos) / sizeof(nombresArchivos[0])];
float num_pasos, num_variables, aux, contador = 0.0f;
std::vector<float> variablesLinea;
bool animate = false;


SpotLight auxSpot = SpotLight();


//DERRAPE CARRO



// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";





void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};


	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

////////////////////////////////////
/// VARIABLES DE CONTROL PARA CUALQUIER COSA
////////////////////////////////////
bool personaje = true;


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	
	mainWindow.linkBools(&personaje);
	CreateObjects();
	CreateShaders();
	//Carga de animaciones
	int numAnimaciones = sizeof(nombresArchivos) / sizeof(nombresArchivos[0]);
	for (int i = 0; i < numAnimaciones; i++) {
		file.open(nombresArchivos[i], std::ifstream::in);
		while (!file.eof()) {
			getline(file, line);
			if (file.good()) {
				stream.clear();
				stream.str(line);
				while (stream.good()) {
					stream >> word;
					//Aqui asignamos los primeros valores que permiten construir el keyframe
					if (primerLinea) {
						if (contador == 0) {
							num_pasos = std::stof(word.c_str());
							contador = 1;
						}
						else {
							num_variables = std::stof(word.c_str());
						}
					}
					else {
						aux = std::stof(word.c_str());
						variablesLinea.push_back(aux);
					}
				}
			}
			if (primerLinea) {
				animaciones[i] = Keyframe(num_pasos, num_variables);
			}
			else {
				animaciones[i].almacenaPasos(variablesLinea);
			}
			primerLinea = false;
			variablesLinea.clear();
			contador = 0;
		}
		primerLinea = true;
		file.close();
	}
	

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.2f, 0.5f);
	cameraHeli = Camera(glm::vec3(-10.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.2f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();

	

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/carro.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/Llanta.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Camino_M = Model();
	Camino_M.LoadModel("Models/pista.obj");
	Lampara_M = Model();
	Lampara_M.LoadModel("Models/LamparaAzul.obj");

	//Nuestras imágenes de atrás y delante (texturas) están invertidas con respecto a la creación del skybox
	std::vector<std::string> madrugadaCaras, diaCaras, atardecerCaras, nocheCaras;
	madrugadaCaras.push_back("Textures/Skybox/madrugada_right.tga");
	madrugadaCaras.push_back("Textures/Skybox/madrugada_left.tga");
	madrugadaCaras.push_back("Textures/Skybox/madrugada_bottom.tga");
	madrugadaCaras.push_back("Textures/Skybox/madrugada_top.tga");
	madrugadaCaras.push_back("Textures/Skybox/madrugada_front.tga");
	madrugadaCaras.push_back("Textures/Skybox/madrugada_back.tga");

	diaCaras.push_back("Textures/Skybox/dia_right.tga");
	diaCaras.push_back("Textures/Skybox/dia_left.tga");
	diaCaras.push_back("Textures/Skybox/dia_bottom.tga");
	diaCaras.push_back("Textures/Skybox/dia_top.tga");
	diaCaras.push_back("Textures/Skybox/dia_front.tga");
	diaCaras.push_back("Textures/Skybox/dia_back.tga");

	atardecerCaras.push_back("Textures/Skybox/atardecer_right.tga");
	atardecerCaras.push_back("Textures/Skybox/atardecer_left.tga");
	atardecerCaras.push_back("Textures/Skybox/atardecer_bottom.tga");
	atardecerCaras.push_back("Textures/Skybox/atardecer_top.tga");
	atardecerCaras.push_back("Textures/Skybox/atardecer_front.tga");
	atardecerCaras.push_back("Textures/Skybox/atardecer_back.tga");

	nocheCaras.push_back("Textures/Skybox/noche_right.tga");
	nocheCaras.push_back("Textures/Skybox/noche_left.tga");
	nocheCaras.push_back("Textures/Skybox/noche_bottom.tga");
	nocheCaras.push_back("Textures/Skybox/noche_top.tga");
	nocheCaras.push_back("Textures/Skybox/noche_front.tga");
	nocheCaras.push_back("Textures/Skybox/noche_back.tga");

	madrugada = Skybox(madrugadaCaras);
	dia = Skybox(diaCaras);
	atardecer = Skybox(atardecerCaras);
	noche = Skybox(nocheCaras);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.2f, 0.0f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	unsigned int spotLightCount = 0;
	//luz helicóptero
	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 3.0f, -2.5f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);

	


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	animate = true;

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		if (personaje) {
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
		else {
			cameraHeli.keyControl(mainWindow.getsKeys(), deltaTime);
			cameraHeli.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
		

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Lógica ciclo dia noche
		if (now >= 0.0f && now < 15.0f) {
			mainLight.setAintensity(0.4);
			madrugada.DrawSkybox(camera.calculateViewMatrix(), projection);
		}
		else if (now >= 15.0f && now < 30.0f) {
			mainLight.setAintensity(0.8);
			dia.DrawSkybox(camera.calculateViewMatrix(), projection);
			//camera.changeYawPitch(90.0f, 0.0f);
		}
		else if (now >= 30.0f && now < 45.0f) {
			mainLight.setAintensity(0.6);
			atardecer.DrawSkybox(camera.calculateViewMatrix(), projection);
		}
		else if (now >= 45.0f && now < 60.0f) {
			mainLight.setAintensity(0.2);
			noche.DrawSkybox(camera.calculateViewMatrix(), projection);
		}
		else {
			glfwSetTime(0.0f);
		}

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		if (personaje) {
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		}
		else {
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(cameraHeli.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, cameraHeli.getCameraPosition().x, cameraHeli.getCameraPosition().y, cameraHeli.getCameraPosition().z);
		}

		
		glm::mat4 model(1.0);
		// luz ligada a la cámara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		//spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		////////////////////////////////////
		/// Modelos de personaje y avión
		////////////////////////////////////

		//Inicia información de personaje
		animaciones[0].animate(&animate);
		model = glm::mat4(1.0);
		model = glm::translate(model, camera.getCameraPosition());
		model = glm::rotate(model, -camera.getYaw() * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kitt_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.0f + animaciones[0].getValor(0), -1.4f, 0.5f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kitt_M.RenderModel();

		/*glm::mat4 model(1.0);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();*/


		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, -2.0f, 5.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(2.0, 1.0f, 2.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Camino_M.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}