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

//Mis texturas
Texture waterTexture;
Texture grassTexture;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture dadoTexture;
Texture dadoDoceTexture;

//Mis modelos
Model Arbol;
Model Arbol1;
Model Arbusto;
Model Bender;
Model Cerca;
Model Isla;
Model Helipuerto;

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

void CrearDado()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};

	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
// average normals
	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.51f,  0.34f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.74f,	0.34f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.74f,	0.65f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.51f,	0.65f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.75f,  0.33f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	1.0f,	0.33f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	1.0f,	0.66f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.75f,	0.66f,		-1.0f,	0.0f,	0.0f,

		// back
		-0.5f, -0.5f, -0.5f,	0.0f,  0.33f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.25f,	0.33f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.25f,	0.66f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f,	0.66f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.25f,  0.33f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.5f,	0.33f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.5f,	0.66f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.25f,	0.66f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.25f,  0.0f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.5f,	0.0f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.5f,	0.33f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.25f,	0.33f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.5f,  0.66f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.75f,	0.66f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.75f,	1.0f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.5f,	1.0f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(dado);
}

void CrearDodecaedro()
{
	unsigned int dodecaedro_indices[] = {
		// Cara 1
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		// Cara 4
		5, 6, 7,
		5, 7, 8,
		5, 8, 9,
		// Cara 6
		10, 11, 12,
		10, 12, 13,
		10, 13, 14,
		// Cara 5
		15, 16, 17,
		15, 17, 18,
		15, 18, 19,
		// Cara 3
		20, 21, 22,
		20, 22, 23,
		20, 23, 24,
		// Cara 2
		25, 26, 27,
		25, 27, 28,
		25, 28, 29,
		// Cara 11
		30, 31, 32,
		30, 32, 33,
		30, 33, 34,
		// Cara 9
		35, 36, 37,
		35, 37, 38,
		35, 38, 39,
		// Cara 7
		40, 41, 42,
		40, 42, 43,
		40, 43, 44,
		// Cara 8
		45, 46, 47,
		45, 47, 48,
		45, 48, 49,
		// Cara 10
		50, 51, 52,
		50, 52, 53,
		50, 53, 54,
		// Cara 12
		55, 56, 57,
		55, 57, 58,
		55, 58, 59
	};

	GLfloat dodecaedro_vertices[] = {
		// front
		// X	      Y		       Z		  U		      V          Nx          Ny          NZ
		//Cara 1
		 1.620f,	0.900f,		-2.750f,	0.580f,		0.199f,		-0.530f,	-0.720f,	0.450f,//0
		 0.000f,	2.080f,		-2.750f,	0.426f,		0.125f,		-0.530f,	-0.720f,	0.450f,//1
		 0.000f,	3.130f,		-1.050f,	0.484f,		0.002f,		-0.530f,	-0.720f,	0.450f,//2
		 1.620f,	2.600f,		 0.000f,	0.678f,		0.002f,		-0.530f,	-0.720f,	0.450f,//3
		 2.620f,	1.230f,		-1.050f,	0.734f,		0.123f,		-0.530f,	-0.720f,	0.450f,//4

		//Cara 2
		 0.000f,	2.080f,		-2.750f,	0.420f,		0.133f,		 0.000f,	 0.000f,	1.000f,//5
		 1.620f,	0.900f,		-2.750f,	0.572f,		0.207f,		 0.000f,	 0.000f,	1.000f,//6
		 1.000f,   -1.000f,	    -2.750f,	0.516f,		0.326f,		 0.000f,	 0.000f,	1.000f,//7
		-1.000f,   -1.000f,		-2.750f,	0.324f,		0.326f,		 0.000f,	 0.000f,	1.000f,//8
		-1.620f,	0.900f,		-2.750f,	0.266f,		0.207f,		 0.000f,	 0.000f,	1.000f,//9

		//Cara 3
		 0.000f,	2.080f,		-2.750f,	0.410f,		0.123f,		 0.530f,	-0.720f,	0.450f,//10
		-1.620f,	0.900f,		-2.750f,	0.258f,		0.197f,		 0.530f,	-0.720f,	0.450f,//11
		-2.620f,	1.230f,		-1.050f,	0.104f,		0.123f,	 	 0.530f,	-0.720f,	0.450f,//12
		-1.620f,	2.600f,		 0.000f,	0.160f,		0.002f,		 0.530f,	-0.720f,	0.450f,//13
		 0.000f,	3.130f,		-1.050f,	0.354f,		0.002f,		 0.530f,	-0.720f,	0.450f,//14

		//Cara 4
		 1.000f,   -1.000f,		-2.750f,	0.527f,		0.330f,		-0.850f,	 0.280f,	0.450f,//15
		 1.620f,	0.900f,		-2.750f,	0.584f,		0.209f,	    -0.850f,	 0.280f,	0.450f,//16
		 2.620f,	1.230f,	    -1.050f,	0.775f,		0.209f,		-0.850f,	 0.280f,	0.450f,//17
		 2.620f,   -0.470f,		 0.000f,	0.836f,		0.330f,		-0.850f,	 0.280f,	0.450f,//18
		 1.620f,   -1.850f,		-1.050f,	0.682f,		0.406f,		-0.850f,	 0.280f,	0.450f,//19

		//Cara 5
		 1.000f,	1.750f,		 1.700f,	0.742f,		0.801f,		 0.000f,	-0.890f,   -0.450f,//20
		 1.620f,	2.600f,		 0.000f,	0.896f,		0.877f,		 0.000f,	-0.890f,   -0.450f,//21
		 0.000f,	3.130f,		-1.050f,	0.838f,		0.998f,		 0.000f,	-0.890f,   -0.450f,//22
		-1.620f,	2.600f,		 0.000f,	0.646f,		0.998f,		 0.000f,	-0.890f,   -0.450f,//23
		-1.000f,	1.750f,		 1.700f,	0.590f,		0.875f,		 0.000f,	-0.890f,   -0.450f,//24

		//Cara 6
		 1.620f,   -0.150f,		 1.700f,	0.689f,		0.668f,		-0.850f,	-0.280f,   -0.450f,//25
		 2.620f,   -0.470f,		 0.000f,	0.842f,		0.594f,		-0.850f,	-0.280f,   -0.450f,//26
		 2.620f,	1.230f,		-1.050f,	0.996f,		0.670f,		-0.850f,	-0.280f,   -0.450f,//27
		 1.620f,	2.600f,		 0.000f,	0.939f,		0.791f,		-0.850f,	-0.280f,   -0.450f,//28
		 1.000f,	1.750f,		 1.700f,	0.746f,		0.791f,		-0.850f,	-0.280f,   -0.450f,//29

		 //Cara 7
		-1.620f,	0.900f,		-2.750f,	0.254f,		0.209f,		 0.850f,	 0.280f,	0.450f,//30
		-1.000f,   -1.000f,		-2.750f,	0.311f,		0.330f,		 0.850f,	 0.280f,	0.450f,//31
		-1.620f,   -1.850f,		-1.050f,	0.156f,		0.406f,		 0.850f,	 0.280f,	0.450f,//32
		-2.620f,   -0.470f,		 0.000f,	0.004f,		0.332f,		 0.850f,	 0.280f,	0.450f,//33
		-2.620f,	1.230f,		-1.050f,	0.061f,		0.209f,		 0.850f,	 0.280f,	0.450f,//34

		//Cara 8
		-1.000f,   -1.000f,		-2.750f,	0.322f,		0.338f,		 0.000f,	 0.890f,	0.450f,//35
		 1.000f,   -1.000f,		-2.750f,	0.516f,		0.338f,		 0.000f,	 0.890f,	0.450f,//36
		 1.620f,   -1.850f,		-1.050f,	0.574f,		0.457f,		 0.000f,	 0.890f,	0.450f,//37
		 0.000f,   -2.380f,		 0.000f,	0.418f,		0.533f,		 0.000f,	 0.890f,	0.450f,//38
		-1.620f,   -1.850f,		-1.050f,	0.266f,		0.459f,		 0.000f,	 0.890f,	0.450f,//39

		//Cara 9
		-1.000f,	1.750f,		 1.700f,	0.572f,		0.877f,		 0.850f,	-0.280f,   -0.450f,//40
		-1.620f,	2.600f,		 0.000f,	0.514f,		0.998f,		 0.850f,	-0.280f,   -0.450f,//41
		-2.620f,	1.230f,		-1.050f,	0.322f,		0.998f,		 0.850f,	-0.280f,   -0.450f,//42
		-2.620f,   -0.470f,		 0.000f,	0.266f,		0.875f,		 0.850f,	-0.280f,   -0.450f,//43
		-1.620f,   -0.150f,		 1.700f,	0.420f,		0.801f,		 0.850f,	-0.280f,   -0.450f,//44

		//Cara 10
		 0.000f,   -1.320f,		 1.700f,	0.484f,		0.662f,		-0.530f,	 0.720f,   -0.450f,//45
		 0.000f,   -2.380f,		 0.000f,	0.428f,		0.541f,		-0.530f,	 0.720f,   -0.450f,//46
		 1.620f,   -1.850f,		-1.050f,	0.582f,		0.467f,		-0.530f,	 0.720f,   -0.450f,//47
		 2.620f,   -0.470f,		 0.000f,	0.736f,		0.541f,		-0.530f,	 0.720f,   -0.450f,//48
		 1.620f,   -0.150f,		 1.700f,	0.676f,		0.662f,		-0.530f,	 0.720f,   -0.450f,//49

		//Cara 11
		 0.000f,   -1.320f,		 1.700f,	0.484f,		0.672f,		 0.000f,	 0.000f,   -1.000f,//50
		 1.620f,   -0.150f,		 1.700f,	0.676f,		0.672f,		 0.000f,	 0.000f,   -1.000f,//51
		 1.000f,	1.750f,		 1.700f,	0.734f,		0.793f,		 0.000f,	 0.000f,   -1.000f,//52
		-1.000f,	1.750f,		 1.700f,	0.580f,		0.869f,		 0.000f,	 0.000f,   -1.000f,//53
		-1.620f,   -0.150f,		 1.700f,	0.428f,		0.793f,		 0.000f,	 0.000f,   -1.000f,//54

		//Cara 12
		-1.620f,   -0.150f,		 1.700f,	0.414f,		0.791f,		 0.530f,	 0.720f,   -0.450f,//55
		-2.620f,   -0.470f,		 0.000f,	0.225f,		0.791f,		 0.530f,	 0.720f,   -0.450f,//56
		-1.620f,   -1.850f,		-1.050f,	0.166f,		0.670f,		 0.530f,	 0.720f,   -0.450f,//57
		 0.000f,   -2.380f,		 0.000f,	0.318f,		0.594f,		 0.530f,	 0.720f,   -0.450f,//58
		 0.000f,   -1.320f,		 1.700f,	0.473f,		0.670f,		 0.530f,	 0.720f,   -0.450f//59


	};
	Mesh* dodecaedro = new Mesh();
	dodecaedro->CreateMesh(dodecaedro_vertices, dodecaedro_indices, 600, 108);
	meshList.push_back(dodecaedro);
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
	CrearDado();
	CrearDodecaedro();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f);

	//Mis texturas
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
	dadoTexture = Texture("Textures/mi_dado.tga");
	dadoTexture.LoadTextureA();
	dadoDoceTexture = Texture("Textures/dodecaedro.tga");
	dadoDoceTexture.LoadTextureA();

	//Mis modelos
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
	skyboxFaces.push_back("Textures/Skybox/skybox_right.tga");
	skyboxFaces.push_back("Textures/Skybox/skybox_left.tga");
	skyboxFaces.push_back("Textures/Skybox/skybox_bottom.tga");
	skyboxFaces.push_back("Textures/Skybox/skybox_top.tga");
	skyboxFaces.push_back("Textures/Skybox/skybox_back.tga");
	skyboxFaces.push_back("Textures/Skybox/skybox_front.tga");

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
		glm::mat4 roadaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		//Modelo del suelo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		waterTexture.UseTexture();
		meshList[2]->RenderMesh();

		//Modelo de avatar Bender
		model = glm::mat4(1.0);
		roadaux = model;
		model = glm::translate(model, glm::vec3(8.0f, 0.1f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Bender.RenderModel();

		//Modelo de los caminos
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
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 15.0));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -15.0));
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

		////Modelo de tiranosaurio
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, -2.0f, -20.0f));
		//color = glm::vec3(0.13f, 0.5f, 0.18f);
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//Tiranosaurio.RenderModel();

		////Modelo de cuello largo
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, -2.0f, 30.0f));
		//color = glm::vec3(0.13f, 0.5f, 0.4f);
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//Brachiosaurus.RenderModel();

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

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}
