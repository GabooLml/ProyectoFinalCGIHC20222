/* Proyecto Final Semestre 2022-2
Autores:
	- Cruz Schouten Max Bernardo
	-  Rojas Méndez Gabriel
*/

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

//para probar el importer
//#include<assimp/Importer.hpp>
#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//Para ilumininación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;
float movCoche;
float movOffset;
float movCocheX;
float movCocheZ;
float rotCoche1;
float rotLlanta;
float rotLlantaOffeset;
float rotHelices;
float rotHelicesOffset;
float movHelicopteroX;
float movHelicopteroOffset;
float rotHelicoptero;
float angulo;
float anguloOffset;
float angulo1;
float anguloOffset1;
bool avanza;
bool avanzaH;
bool avanzaC;
int luz;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture dadoTexture;
Texture dadoDoceTexture;

Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;
Model Dado_M;
Model Mi_Auto;
Model Mi_Auto1;
Model Mi_Llanta;
Model Helice;
Model Helice_T;
Model Tiranosaurio;
Model Brachiosaurus;
Model Poste;
Model Faro;
Model Pista;

Skybox skybox;

//Materiales
Material Material_brillante;
Material Material_opaco;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

//Luz direccional
DirectionalLight mainLight;

//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

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

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);
}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/helicoptero.obj");
	Helice = Model();
	Helice.LoadModel("Models/helice.obj");
	Helice_T = Model();
	Helice_T.LoadModel("Models/helice_trasera.obj");
	Mi_Auto = Model();
	Mi_Auto.LoadModel("Models/LowPoly Muscle Cougar xr1970.obj");
	Mi_Auto1 = Model();
	Mi_Auto1.LoadModel("Models/muscle.obj");
	Mi_Llanta = Model();
	Mi_Llanta.LoadModel("Models/llanta_muscle.obj");
	Tiranosaurio = Model();
	Tiranosaurio.LoadModel("Models/tiranosaurio.obj");
	Brachiosaurus = Model();
	Brachiosaurus.LoadModel("Models/cuello_largo.obj");
	Dado_M = Model();
	Dado_M.LoadModel("Models/mi_dado.obj");
	Poste = Model();
	Poste.LoadModel("Models/Poste.obj");
	Faro = Model();
	Faro.LoadModel("Models/Lampara.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.8f,
		0.0f, 0.0f, -1.0f);

	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		1.0f, 2.0f,
		2.0f, 1.5f, 20.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;

	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//Luz de auto trasera
	spotLights[1] = SpotLight(1.0f, 0.0f, 0.0f,
		1.0f, 2.0f,
		0.0f, 10.0f, 2.5f,
		0.5f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	//spotLightCount++;

	//luz de auto delantera
	spotLights[2] = SpotLight(1.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		0.0f, 10.0f, 2.5f,
		-0.5f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	//spotLightCount++;

	//luz helicoptero
	spotLights[3] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, -10.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	//spotLightCount++;

	//Vista
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	//Variables para las animaciones
	movCoche = 0.0f;
	movCocheX = 0.0f;
	movCocheZ = 0.0f;
	rotCoche1 = 0.0f;
	movOffset = 0.15f;
	rotLlanta = 0.0;
	rotLlantaOffeset = 0.15;
	rotHelices = 0.0f;
	rotHelicesOffset = 0.15f;
	movHelicopteroX = 0.0f;
	movHelicopteroOffset = 0.25f;
	rotHelicoptero = 0.0f;
	angulo = 0.0f;
	anguloOffset = 0.05;
	angulo1 = 0.0f;
	anguloOffset1 = 0.01;
	
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Control de luces puntuales
		switch (mainWindow.getLight())
		{
		case 1:
			pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,
				1.0f, 2.0f,
				-15.0f, 16.0f, 20.0f,
				0.3f, 0.2f, 0.1f);
			pointLightCount = 2;
			break;

		case 2:
			pointLights[1] = PointLight(1.0f, 0.87f, 0.17f,
				1.0f, 2.0f,
				-15.0f, 16.0f, 20.0f,
				0.3f, 0.2f, 0.1f);
			pointLightCount = 2;
			break;

		case 3:
			pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,
				1.0f, 2.0f,
				-15.0f, 16.0f, 20.0f,
				0.3f, 0.2f, 0.1f);

			pointLights[2] = PointLight(1.0f, 0.87f, 0.17f,
				1.0f, 2.0f,
				-15.0f, 16.0f, 20.0f,
				0.3f, 0.2f, 0.1f);
			pointLightCount = 3;
			break;

		default:
			pointLightCount = 1;
			break;
		}

		////Control luz helicoptero
		if (mainWindow.getLightLamp() == 0)
		{
			spotLightCount = 4;
		}
		else
		{
			spotLightCount = 3;
		}

		//Movimiento helicoptero
		if (movHelicopteroX < 50.0f and avanzaH == false)
		{
			movHelicopteroX += movHelicopteroOffset * deltaTime;
			angulo += anguloOffset * deltaTime;
			if (angulo > 359.0f)
			{
				angulo = 0.0f;
			}

			if (movHelicopteroX > 49.0f)
			{
				avanzaH = true;
				rotHelicoptero = 0.0f;
			}
		}
		else {
			movHelicopteroX -= movHelicopteroOffset * deltaTime;
			angulo += anguloOffset * deltaTime;
			if (angulo > 359.0f)
			{
				angulo = 0.0f;
			}
		
			if (movHelicopteroX < -49.0f)
			{
				avanzaH = false;
				rotHelicoptero = 180.0f;
			}
		}

		//Movimiento del coche 1
		if (angulo1 < 180.0f and avanzaC == false)
		{
			angulo1 += anguloOffset1 * deltaTime;
			if (angulo1 > 179.0f)
			{
				avanzaC = true;
			}
		}
		else
		{
			angulo1 -= anguloOffset1 * deltaTime;
			if (angulo1 < 1.0f)
			{
				avanzaC = false;
			}
		}
		movCocheX = -120 + 50 * cos(angulo1);
		movCocheZ = -45 + 20 * sin(angulo1);

		//Giro para la helice
		if (rotHelices > 359.0f)
		{
			rotHelices = 0.0f;
		}
		rotHelices += rotHelicesOffset * deltaTime;

		//Desplazamiento del vehiculo
		if (movCoche < 90.0f and avanza == false)
		{
			movCoche += movOffset * deltaTime;
			rotLlanta += rotLlantaOffeset * deltaTime;
			luz = 1;
			if (movCoche > 89.0f)
			{
				avanza = true;
			}
		}
		else {
			movCoche -= movOffset * deltaTime;
			rotLlanta -= rotLlantaOffeset * deltaTime;
			luz = 2;
			if (movCoche < -89.0f)
			{
				avanza = false;
			}
		}

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
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
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition(); //captura la posicion de la camara
		lowerLight.y -= 0.3f; //Baja tres unidades de la posicion de la camara para que nos vea raro
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection()); //Modifica la posicion de la camara

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		//Modelo del suelo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoTexture.UseTexture();
		meshList[2]->RenderMesh();
		
		//Mi auto
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.1f, 9.0));
		model = glm::translate(model, glm::vec3(movCoche, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getCarLap()), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Mi_Auto1.RenderModel();

		spotLights[luz].SetPos(glm::vec3(5.0f + movCoche, 2.5f, 9.0f));

		//Llanta izquierda delantera
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.6f, 0.4f, 1.5));
		model = glm::rotate(model, -rotLlanta, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Mi_Llanta.RenderModel();

		//Llanta izquierda trasera
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.525f, 0.4f, 1.5));
		model = glm::rotate(model, -rotLlanta, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Mi_Llanta.RenderModel();

		//Llanta derecha delantera
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.6f, 0.4f, -1.4));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotLlanta, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Mi_Llanta.RenderModel();

		//Llanta derecha trasera
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.525f, 0.4f, -1.4));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotLlanta, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Mi_Llanta.RenderModel();

		//Modelo del vehículo
		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(30.0f, -0.6f, -1.5f));
		model = glm::translate(model, glm::vec3(movCocheX, 0.0f, -movCocheZ));
		model = glm::rotate(model, angulo1, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		color = glm::vec3(1.0f, 0.0f, 0.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Kitt_M.RenderModel();

		//Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.7f, 0.0f, 2.0f));
		color = glm::vec3(0.5f, 0.5f, 0.5);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Llanta_M.RenderModel();

		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 2.0f));
		color = glm::vec3(0.7f, 0.5f, 0.7);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Llanta_M.RenderModel();

		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, -1.2f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		color = glm::vec3(0.2f, 0.5f, 0.7);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Llanta_M.RenderModel();

		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.7f, 0.0f, -1.2f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		color = glm::vec3(0.2f, 0.7f, 0.7);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Llanta_M.RenderModel();

		//Render del helicóptero
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(25.0f, 15.0f, -15.0f));
		model = glm::translate(model, glm::vec3(movHelicopteroX, 2 * sin(angulo), mainWindow.getHelicopterZ()));
		model = glm::rotate(model, glm::radians(rotHelicoptero), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();

		//Rotor central
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
		model = glm::rotate(model, rotHelices, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Helice.RenderModel();

		//Rotor trasero
		model = modelaux;
		model = glm::translate(model, glm::vec3(10.35f, 2.3f, -0.25));
		model = glm::rotate(model, rotHelices, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Helice_T.RenderModel();

		//Luz del helicoptero
		spotLights[3].SetPos(glm::vec3(25.0f + movHelicopteroX, 15.0, -15.0f + mainWindow.getHelicopterZ()));

		//Modelo del faro del helicoptero
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(25.0f, -0.1f, -35.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Faro.RenderModel();

		//Modelo de tiranosaurio
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, -20.0f));
		color = glm::vec3(0.13f, 0.5f, 0.18f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Tiranosaurio.RenderModel();

		//Modelo de cuello largo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 30.0f));
		color = glm::vec3(0.13f, 0.5f, 0.4f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Brachiosaurus.RenderModel();

		//Cubo de OpenGL
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 5.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		dadoTexture.UseTexture();
		meshList[4]->RenderMesh();

		//Dado de Blender
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-20.0f, 5.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Dado_M.RenderModel();

		//Dodecaedro
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 10.0f, 20.0f));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		dadoDoceTexture.UseTexture();
		meshList[5]->RenderMesh();

		//Poste 1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-30.0f, 0.0f, 18.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Poste.RenderModel();

		//Poste 2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-30.0f, 0.0f, -8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Poste.RenderModel();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}
