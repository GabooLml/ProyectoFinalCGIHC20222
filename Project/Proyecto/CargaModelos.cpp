/* Proyecto Final Semestre 2022-2
Autores:
	- Cruz Schouten Max Bernardo
	- Rojas Méndez Gabriel
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
//Para animaciones y archivos
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

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
#include "Keyframe.h"
#include "ControlCamaras.h"
//PARA LA IMPLEMENTACIÓN DE SONIDO
#include <irrKlang.h>
using namespace irrklang;

//Para ilumininación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;
//Variables para animación del tiranosaurio
float movPiernaRT;
float movPiernaLT;
float movOffsetT;
float rotColaT;
float rotColaTOffset;
bool movTirano;
bool movTiranoCola;
float toffsetu;
float toffsetv;

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
bool camPersonaje = true;
int luz;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;


//Mis texturas
Texture waterTexture;
Texture grassTexture;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;


//SONIDOS
ISoundEngine* Fondo = createIrrKlangDevice();

//Mis modelos
Model Arbol;
Model Arbol1;
Model Arbusto;
Model Bender;
Model Cerca;
Model Isla;
Model Helipuerto;
Model Tiranosaurio;
Model PiernaRT;
Model PiernaLT;
Model ColaT;
Model CuelloL;
Model Stegosaurus;
Model Velociraptor;

Model Blackhawk_M;
Model Helice;
Model Helice_T;
Model Poste;

//Camaras
Camera camera, cameraHeli;
glm::vec3 posPersonaje = glm::vec3(8.0f, 0.1f, 7.2f), posHeli = glm::vec3(0.0f,2.0f,-40.0f);

//Skybox
Skybox madrugada, dia, atardecer, noche;


//Materiales
Material Material_brillante;
Material Material_opaco;

//VARIABLES PARA EJECUCIÓN HOMOGENEA EN CUALQUIER EQUIPO
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

//Luz direccional
DirectionalLight mainLight;

//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

/// elementos para el acceso de archivos de animaciones y keyframes

std::string filename;
std::ifstream file;
std::istringstream stream;
std::string line;
std::string word;
bool primerLinea = true;
std::string nombresArchivos[] = { "helicoptero.txt" };
Keyframe animaciones[sizeof(nombresArchivos) / sizeof(nombresArchivos[0])];
float num_pasos, num_variables, aux, contador = 0.0f;
std::vector<float> variablesLinea;
bool animate, animacionHelicoptero = false;

GLfloat inclinacionDerIzq, inclinacionFrentAtr = 0.0f;


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

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	float rotacionP = 0.0f;
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	mainWindow.linkBools(&camPersonaje, &animacionHelicoptero);

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

	camera = Camera(posPersonaje, glm::vec3(0.0f, 1.0f, 0.0f),&posPersonaje, -90.0f, -20.0f, 0.2f, 0.5f, &camPersonaje);
	cameraHeli = Camera(posHeli, glm::vec3(0.0f, 1.0f, 0.0f), &posHeli, 90.0f, -20.0f, 0.2f, 0.5f, &camPersonaje);
	camera.keyControl(mainWindow.getsKeys(), deltaTime);
	cameraHeli.setAnimacionPtr(&animaciones[0]);
	//controlCamaras = ControlCamaras();

	//TEXTURAS
	waterTexture = Texture("Textures/agua.tga");
	waterTexture.LoadTextureA();
	grassTexture = Texture("Textures/cesped.tga");
	grassTexture.LoadTextureA();

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();

	//MODELOS
	Arbol = Model();
	Arbol.LoadModel("Models/arbol1.obj");
	Arbol1 = Model();
	Arbol1.LoadModel("Models/arbol2.obj");
	Arbusto = Model();
	Arbusto.LoadModel("Models/Arbusto.obj");
	Bender = Model();
	Bender.LoadModel("Models/Bender.obj");
	Isla = Model();
	Isla.LoadModel("Models/isla.obj");
	Helipuerto = Model();
	Helipuerto.LoadModel("Models/helipuerto.obj");
	Tiranosaurio = Model();
	Tiranosaurio.LoadModel("Models/tiranoVP.obj");
	PiernaRT = Model();
	PiernaRT.LoadModel("Models/piernaRTirano2.obj");
	PiernaLT = Model();
	PiernaLT.LoadModel("Models/piernaLTirano3.obj");
	ColaT = Model();
	ColaT.LoadModel("Models/colaTiranosaurio1.obj");
	CuelloL = Model();
	CuelloL.LoadModel("Models/cuelloLargoVC.obj");
	Stegosaurus = Model();
	Stegosaurus.LoadModel("Models/stegosaurusVC.obj");
	Velociraptor = Model();
	Velociraptor.LoadModel("Models/velociraptorVC.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/helicoptero.obj");
	Helice = Model();
	Helice.LoadModel("Models/helice.obj");
	Helice_T = Model();
	Helice_T.LoadModel("Models/helice_trasera.obj");
	Poste = Model();
	Poste.LoadModel("Models/Poste.obj");


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
	GLuint uniformColor = 0, uniformTextureOffset = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	//Variables para animación del tiranosaurio
	movPiernaRT = 0.0f;
	movPiernaLT = 0.0f;
	movOffsetT = 0.05;
	rotColaT = 0.0f;
	rotColaTOffset = 0.05;
	toffsetu = 0.0f;
	toffsetv = 0.0f;

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

	//MUSICA DE FONDO
	Fondo->play2D("Music/Fondo.mp3", true);
	
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		animaciones[0].animate(&animacionHelicoptero);
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

		/*Movimiento helicoptero
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
		}*/

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
		/*if (rotHelices > 359.0f)
		{
			rotHelices = 0.0f;
		}
		rotHelices += rotHelicesOffset * deltaTime;*/

		////Desplazamiento del vehiculo
		//if (movCoche < 90.0f and avanza == false)
		//{
		//	movCoche += movOffset * deltaTime;
		//	rotLlanta += rotLlantaOffeset * deltaTime;
		//	luz = 1;
		//	if (movCoche > 89.0f)
		//	{
		//		avanza = true;
		//	}
		//}
		//else {
		//	movCoche -= movOffset * deltaTime;
		//	rotLlanta -= rotLlantaOffeset * deltaTime;
		//	luz = 2;
		//	if (movCoche < -89.0f)
		//	{
		//		avanza = false;
		//	}
		//}

		////Animación para tiranosaurio
		//if (movPiernaRT < 30.0 and movTirano == false)
		//{
		//	movPiernaRT += movOffsetT * deltaTime;
		//	//movPiernaLT -= movOffsetT * deltaTime;
		//	if (movPiernaRT > 29.0f);
		//	{
		//		movTirano = true;
		//	}
		//}
		//else
		//{
		//	movPiernaRT -= movOffsetT * deltaTime;
		//	//movPiernaLT += movOffsetT * deltaTime;
		//	if (movPiernaRT < -29.0f);
		//	{
		//		movTirano = false;
		//	}
		//}
		
		if (rotacionP < 10.0 and movTirano == false)
		{
			rotacionP += movOffsetT * deltaTime;
			if (rotacionP > 9.0)
			{
				movTirano = true;
			}
		}
		else
		{
			movPiernaRT -= movOffsetT * deltaTime;
			movPiernaLT += movOffsetT * deltaTime;
			if (movPiernaRT > -29.0f);
			{
				movTirano = false;
			}
		}

		//Animación de la cola del tiranosaurio
		if (rotColaT < 10.0f and movTiranoCola == false)
		{
			rotColaT += rotColaTOffset * deltaTime;
			if (rotColaT > 9.0f)
			{
				movTiranoCola = true;
			}
		}
		else
		{
			rotColaT -= rotColaTOffset * deltaTime;
			if (rotColaT < -9.0f)
			{
				movTiranoCola = false;
			}
		}

		//Recibir eventos del usuario
		glfwPollEvents();

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
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		if (camPersonaje) {
			camera.addYawPitch(mainWindow.getXChange(), 0.0f);
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		}
		else {
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(cameraHeli.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, cameraHeli.getCameraPosition().x, cameraHeli.getCameraPosition().y, cameraHeli.getCameraPosition().z);
		}


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
		glm::mat4 roadaux(1.0);
		glm::mat4 tiranoaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);


		////////////////////////////////// MODELO AVATAR BENDER ///////////////////////////////////
		model = glm::mat4(1.0);
		roadaux = model;
		//printf("Posiciones: %f, %f\n", camera.getFront().x, camera.getFront().z);
		model = glm::translate(model, (posPersonaje - glm::vec3(0.0f,0.0f,0.0f)));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		model = glm::rotate(model, ((90.0f - camera.getYaw())* toRadians), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::translate(model, glm::vec3(camera.geDirecionVector().x, 0.0f,   ));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Bender.RenderModel();
		model = glm::translate(model, glm::vec3(0.0,1.0,-1.0));
		animate = true;

		////////////////////////////////////// GENERACIÓN DE LOS ELEMENTOS DEL ESCENARIO ///////////////////
		////////////////// SUELO, POSTES, ÁRBOLES, ISLAS Y HELIPUERTO ////////////////////////////
		//Variables para el movimiento del agua;
		toffsetu += 0.0001;
		toffsetv += 0.0;
		if (toffsetu > 1.0)
		{
			toffsetu = 0.0;
		}
		toffset = glm::vec2(toffsetu, toffsetv);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f, 1.0f, 15.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		waterTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();
		glDisable(GL_BLEND);
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

		//MODELOS DE CAMINOS
		model = glm::mat4(1.0);
		roadaux = model;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.0f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		grassTexture.UseTexture();
		meshList[2]->RenderMesh();

		model = roadaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -42.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Helipuerto.RenderModel();

		model = roadaux;
		model = glm::translate(model, glm::vec3(30.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		grassTexture.UseTexture();
		meshList[2]->RenderMesh();

		model = roadaux;
		model = glm::translate(model, glm::vec3(-30.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		grassTexture.UseTexture();
		meshList[2]->RenderMesh();

		model = roadaux;
		model = glm::translate(model, glm::vec3(55.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		grassTexture.UseTexture();
		meshList[2]->RenderMesh();

		model = roadaux;
		model = glm::translate(model, glm::vec3(-55.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		grassTexture.UseTexture();
		meshList[2]->RenderMesh();

		model = roadaux;
		model = glm::translate(model, glm::vec3(80.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		grassTexture.UseTexture();
		meshList[2]->RenderMesh();

		//Isla de cuello largo
		model = roadaux;
		model = glm::translate(model, glm::vec3(54.0f, 0.0f, 45.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Isla.RenderModel();

		//Isla de estegosaurio
		model = roadaux;
		model = glm::translate(model, glm::vec3(-54.0f, 0.0f, 45.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Isla.RenderModel();

		//Isla de tiranosaurio
		model = roadaux;
		model = glm::translate(model, glm::vec3(54.0f, 0.0f, -45.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Isla.RenderModel();

		//Isla de velociraptor
		model = roadaux;
		model = glm::translate(model, glm::vec3(-54.0f, 0.0f, -45.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Isla.RenderModel();

		//Árboles de la plaza central
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol1.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(27.5f, 0.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol1.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-27.5f, 0.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol1.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 15.0));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -15.0));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-42.5f, 0.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(42.5f, 0.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol.RenderModel();

		//Postes de la plaza central
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 8.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Poste.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -8.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Poste.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-55.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Poste.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(55.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Poste.RenderModel();

		////////////////////////////////// MODELO HELICOPTERO ////////////////////////////////////////
		//Formato archivo para valores [rotacionRotores, aumento X, aumento Y, Aumento Z, yaw, inclinacionDerIzq, inclinacionFrentAtr ]
		model = glm::mat4(1.0);
		model = glm::translate(model, posHeli);
		model = glm::rotate(model, glm::radians(-cameraHeli.getYaw() + 180), glm::vec3(0.0f, 1.0f, 0.0f));
		if (animacionHelicoptero) {
			cameraHeli.positionControl(&animacionHelicoptero, deltaTime, animaciones[0].getValor(1), animaciones[0].getValor(2), animaciones[0].getValor(3));
			cameraHeli.rotationControl(&animacionHelicoptero, deltaTime, animaciones[0].getValor(4), 0.0f);
			inclinacionDerIzq += animaciones[0].getValor(5);
			inclinacionFrentAtr += animaciones[0].getValor(6);
			model = glm::rotate(model, glm::radians(inclinacionDerIzq), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(inclinacionFrentAtr), glm::vec3(0.0f, 0.0f, 1.0f));
		}		
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();

		//Rotor central
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
		model = glm::rotate(model, animaciones[0].getValor(0), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Helice.RenderModel();

		//Rotor trasero
		model = modelaux;
		model = glm::translate(model, glm::vec3(10.35f, 2.3f, -0.25));
		model = glm::rotate(model, animaciones[0].getValor(0), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Helice_T.RenderModel();

	


		//////////////////////////////////// MODELO DEL TIRANOSAURIO ///////////////////////////////////////
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-55.0f, 0.0f, -20.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		tiranoaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Tiranosaurio.RenderModel();

		//Pierna derecha
		model = tiranoaux;
		model = glm::rotate(model, toRadians * (-rotacionP), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaRT.RenderModel();

		//Pierna izquierda
		model = tiranoaux;
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
		model = glm::rotate(model, toRadians * rotacionP, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaLT.RenderModel();

		//Cola del tiranosaurio
		model = tiranoaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.02f));
		model = glm::rotate(model, toRadians * rotColaT, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ColaT.RenderModel();

		////////////////////////////// MODELO DE CUELLO LARGO ///////////////////////////////////
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-55.0f, 0.0f, 20.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuelloL.RenderModel();

		////////////////////////////// MODELO DE STEGOSAURUS ///////////////////////////////////
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(55.0f, 0.0f, -30.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Stegosaurus.RenderModel();

		////////////////////////////// MODELO DE VELOCIRAPTOR ///////////////////////////////////
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(55.0f, 0.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Velociraptor.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, 0.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Velociraptor.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(60.0f, 0.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Velociraptor.RenderModel();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}

void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_2])
	{
		
	}

}
