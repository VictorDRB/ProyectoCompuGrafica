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
#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animación
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
float toffsetu = 0.0f;
float toffsetv = 0.0f;

float subiendo = 0.0f;
bool up = true;


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture pisoTexture;
Texture Road;
Texture AgaveTexture;
Texture Concreto;
Texture aguaFuente;

Texture aguas_frescas_anuncio;
Texture quesadillas_anuncio;
Texture gorditas_anuncio;


//############ Modelos ############

Model panaderia;
Model puesto_aguas;
Model puesto_quesadillas;
Model quesadilla;
Model jarra;
Model vaso;

//Puesto Tacos
Texture Carne;
Texture MetalShine;
Texture MetalR;
Texture MetalS;
Texture TGuapo;
Texture puestoTacos;



Model Fuente;
Model Blackhawk_M;
Model Tacos;
Model Trompo1;
Model Trompo2;
Model Ramen;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;

//RELOJ
std::clock_t start, finish;
double duration;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
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
	   4, 5, 6,
	   4, 6, 7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		 0.0f, -0.5f, -0.5f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.0f, -0.5f,  0.5f,	1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.0f,  0.5f,  0.5f,	1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		 0.0f,  0.5f, -0.5f,	0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	

	unsigned int anuncioAguasFrescasIndices[] = {
		0, 1, 2,
		0, 2, 3
	};
	GLfloat anuncioAguasFrescasVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f
	};
	unsigned int anuncioQuesadillasIndices[] = {
		0, 1, 2,
		0, 2, 3
	};
	GLfloat anuncioQuesadillasVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f
	};
	unsigned int anuncioGorditasIndices[] = {
		0, 1, 2,
		0, 2, 3
	};
	GLfloat anuncioGorditasVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f
	};

	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		 0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		 0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};


	unsigned int CaminoIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat CaminoVertices[] = {
		-5.0f, 0.0f,  5.0f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		 5.0f, 0.0f,  5.0f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		 5.0f, 0.0f, -5.0f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-5.0f, 0.0f, -5.0f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};


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

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(CaminoVertices, CaminoIndices, 32, 6);
	meshList.push_back(obj6);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


void skybox_function(int i) {
	std::vector<std::string> skyboxFaces;

	if (i == 0) {
		skyboxFaces.push_back("Textures/Skybox/noche_lados.tga");
		skyboxFaces.push_back("Textures/Skybox/noche_lados.tga");
		skyboxFaces.push_back("Textures/Skybox/noche_lados.tga");
		skyboxFaces.push_back("Textures/Skybox/noche_lados.tga");
		skyboxFaces.push_back("Textures/Skybox/noche_lados.tga");
		skyboxFaces.push_back("Textures/Skybox/noche_lados.tga");
		skybox = Skybox(skyboxFaces);
	}
	else {
		skyboxFaces.push_back("Textures/Skybox/ladosDos.tga");
		skyboxFaces.push_back("Textures/Skybox/ladosDos.tga");
		skyboxFaces.push_back("Textures/Skybox/ladosDos.tga");
		skyboxFaces.push_back("Textures/Skybox/arriba.tga");
		skyboxFaces.push_back("Textures/Skybox/ladosDos.tga");
		skyboxFaces.push_back("Textures/Skybox/ladosDos.tga");
		skybox = Skybox(skyboxFaces);
	}
}


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 2.0f, 0.5f);

	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	

	//##### Mis Texturas ############
	pisoTexture = Texture("Textures/Grass.tga");
	pisoTexture.LoadTextureA();
	Road = Texture("Textures/road.tga");
	Road.LoadTextureA();
	aguaFuente = Texture("Textures/aguaFount.tga");
	aguaFuente.LoadTextureA();

	aguas_frescas_anuncio = Texture("Textures/aguas_frescas_anuncio.tga");
	aguas_frescas_anuncio.LoadTextureA();
	quesadillas_anuncio = Texture("Textures/cartel_quesadillas.tga");
	quesadillas_anuncio.LoadTextureA();
	gorditas_anuncio = Texture("Textures/gorditas_anuncio.tga");

	/*Concreto = Texture("Textures/concreto.tga");
	Concreto.LoadTextureA();*/
	
	
	//Puesto Tacos

	Carne = Texture("Textures/PuestoTacos/meat.tga");
	Carne.LoadTextureA();
	MetalShine = Texture("Textures/PuestoTacos/metal_shine.tga");
	MetalShine.LoadTextureA();
	MetalR = Texture("Textures/PuestoTacos/metal_rough.tga");
	MetalR.LoadTextureA();
	MetalS = Texture("Textures/PuestoTacos/metal_shamed1.tga");
	MetalS.LoadTextureA();
	puestoTacos = Texture("Textures/PuestoTacos/puestoTacos.tga");
	puestoTacos.LoadTextureA();
		

	//######################### Models ################################
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	
	//Modelos
	Fuente = Model();
	Fuente.LoadModel("Models/fuenteAgua.obj");
	Tacos = Model();
	Tacos.LoadModel("Models/puestoTacos.obj");
	Trompo1 = Model();
	Trompo1.LoadModel("Models/Trompo1.obj");
	Trompo2 = Model();
	Trompo2.LoadModel("Models/Trompo2.obj");
	Ramen = Model();
	Ramen.LoadModel("Models/puestoRamen.obj");

	//--------------------------------------
	panaderia = Model();
	panaderia.LoadModel("Models/panaderia.obj");
	puesto_aguas = Model();
	puesto_aguas.LoadModel("Models/puesto_aguas_completo.obj");
	puesto_quesadillas = Model();
	puesto_quesadillas.LoadModel("Models/puesto_quesadillas.obj");
	quesadilla = Model();
	quesadilla.LoadModel("Models/quesadilla.obj");
	vaso = Model();
	vaso.LoadModel("Models/vaso.obj");
	jarra = Model();
	jarra.LoadModel("Models/jarra.obj");
	
	


	std::vector<std::string> skyboxFaces;
	// ******************* Dia *******************
	skyboxFaces.push_back("Textures/Skybox/ladosDos.tga");
	skyboxFaces.push_back("Textures/Skybox/ladosDos.tga");
	skyboxFaces.push_back("Textures/Skybox/ladosDos.tga");
	skyboxFaces.push_back("Textures/Skybox/arriba.tga");
	skyboxFaces.push_back("Textures/Skybox/ladosDos.tga");
	skyboxFaces.push_back("Textures/Skybox/ladosDos.tga"); 

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
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

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	//luz de helicóptero

	//luz de faro



	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	movCoche = 0.0f;
	movOffset = 0.01f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;

	
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		if (movCoche < 30.0f)
		{
			movCoche -= movOffset * deltaTime;
			//printf("avanza%f \n ",movCoche);
		}
		rotllanta += rotllantaOffset * deltaTime;


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
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


	//############################# Cesped ################################

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

	//######################### Camino ##################################
		
		float CscZ = 1.5f;
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(275.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, CscZ));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(225.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, CscZ));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(175.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, CscZ));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(125.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, CscZ));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, CscZ));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(25.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, CscZ));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-25.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, CscZ));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-75.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, CscZ));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-125.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, CscZ));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-175.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, CscZ));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-225.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, CscZ));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

	//######################## Fuente ########################################

		float Fsc=7.0f;

		model = glm::mat4(1.0);
		//color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(Fsc, Fsc, Fsc));
		//model = glm::rotate(model, 7 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Fuente.RenderModel();


	//######################## Puesto Tacos ########################################

		float Tsc = 2.0f;
		float TtrX = -80.0, TtrZ= -80.0f;

		model = glm::mat4(1.0);
		//color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::translate(model, glm::vec3(TtrX, -0.9f, TtrZ));
		model = glm::scale(model, glm::vec3(Tsc, Tsc, Tsc));
		//model = glm::rotate(model, 7 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Tacos.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(TtrX, -0.9f, TtrZ));
		model = glm::scale(model, glm::vec3(Tsc, Tsc, Tsc));
		//model = glm::rotate(model, 7 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Trompo1.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(TtrX, -0.9f, TtrZ));
		model = glm::scale(model, glm::vec3(Tsc, Tsc, Tsc));
		//model = glm::rotate(model, 7 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Trompo2.RenderModel();

		//######################## Puesto Ramen ########################################

		float Rsc = 0.8f;
		float RtrX = 80.0, RtrZ = 80.0f;

		model = glm::mat4(1.0);
		//color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::translate(model, glm::vec3(RtrX, -0.9f, RtrZ));
		model = glm::scale(model, glm::vec3(Rsc, Rsc, Rsc));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Ramen.RenderModel();



		// ------------------ PANADERIA ------------------ 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 100.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, mainWindow.getFoodTruckMovimiento())); //ANIMACION BASICA #1
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		panaderia.RenderModel();

		// ------------------ PUESTO AGUAS ------------------ 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(100.0f, 0.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puesto_aguas.RenderModel();
		
		// ******************* ANUNCIO *******************
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(100.0f, 9.0f, 4.7f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		aguas_frescas_anuncio.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh(); //8

		//// ****************** VASO ******************
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(100.0f, 0.0f, 7.0f));
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//vaso.RenderModel();
		//
		//// ****************** JARRA ******************
		//rotllanta += rotllantaOffset * deltaTime;
		//subiendo += 0.001;

		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(100.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, 40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::translate(model, glm::vec3(((sin(glm::radians(rotllanta))) * glm::radians(rotllanta)) * 0.01, subiendo, (-1 + (cos(glm::radians(rotllanta)) * glm::radians(rotllanta))) * 0.01));
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//jarra.RenderModel();

		//// ------------------ PUESTO QUESADILLAS ------------------ 
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(120.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//puesto_quesadillas.RenderModel();
		//// ******************* QUESADILLA *******************
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(120.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		////model = glm::translate(model, glm::vec3(mainWindow.getQuesadillaA(), 0.5f, 0.0f)); //ANIMACION BASICA #2
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//quesadilla.RenderModel();
		//// ******************* ANUNCIO QUESADILLAS *******************
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(130.0f, 9.0f, 4.7f));
		//model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//quesadillas_anuncio.UseTexture();
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[2]->RenderMesh(); //9
		//// ******************* ANUNCIO GORDITAS *******************
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(115.0f, 10.0f, 4.7f));
		//model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//gorditas_anuncio.UseTexture();
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[3]->RenderMesh(); // 10

		

		//textura con movimiento
		//Importantes porque la variable uniform no podemos modificarla directamente
		toffsetu += 0.00;
		toffsetv -= 0.05;
		//para que no se desborde la variable
		if (toffsetu > 1.0)
			toffsetu = 0.0;
		//if (toffsetv > 1.0)
		//	toffsetv = 0;
		//printf("\ntfosset %f \n", toffsetu);
		//pasar a la variable uniform el valor actualizado
		toffset = glm::vec2(toffsetu, toffsetv);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 39.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 20.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		aguaFuente.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();
		glDisable(GL_BLEND);
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 39.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 20.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		aguaFuente.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
