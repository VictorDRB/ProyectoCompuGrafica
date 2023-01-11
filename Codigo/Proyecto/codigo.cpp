#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>r
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
#include "Model.h"
#include "Skybox.h"
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;

//variables para animación
float rotllanta;
float rotllantaOffset;
float subiendo = 0.0f;
bool up = true;
float toffsetu = 0.0f;
float toffsetv = 0.0f;


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture pisoTexture;

Texture aguas_frescas_anuncio;
Texture quesadillas_anuncio;
Texture gorditas_anuncio;
Model panaderia;
Model puesto_aguas;
Model puesto_quesadillas;
Model quesadilla;
Model jarra;
Model vaso;
Model Tacos;
Model Trompo1, Trompo2;
Model Fuente;


Texture Fount1;
Texture Fount2;
Texture Fount3;
Texture Fount4;
Texture Concreto;
Texture Road;
Texture aguaFuente;
Texture Carne, MetalShine, MetalR, MetalS, puestoTacos;

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
GLfloat tiempo = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight, dayLight, nightLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

SpotLight linterna, spotLightOff;

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";
// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount, unsigned int vLength, unsigned int normalOffset)
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
	obj1->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj1);
	Mesh* obj2 = new Mesh(); 
	obj2->CreateMesh(anuncioAguasFrescasVertices, anuncioAguasFrescasIndices, 32, 6);
	meshList.push_back(obj2);
	Mesh* obj3 = new Mesh(); 
	obj3->CreateMesh(anuncioQuesadillasVertices, anuncioQuesadillasIndices, 32, 6);
	meshList.push_back(obj3);
	Mesh* obj4 = new Mesh(); 
	obj4->CreateMesh(anuncioGorditasVertices, anuncioGorditasIndices, 32, 6);
	meshList.push_back(obj4);
	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(CaminoVertices, CaminoIndices, 32, 6);
	meshList.push_back(obj5);
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
	}else{
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

	camera = Camera(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	pisoTexture = Texture("Textures/grass.tga");
	pisoTexture.LoadTextureA();

	// ****************** PROYECTO ******************
	aguas_frescas_anuncio = Texture("Textures/aguas_frescas_anuncio.tga");
	aguas_frescas_anuncio.LoadTextureA();
	quesadillas_anuncio = Texture("Textures/cartel_quesadillas.tga");
	quesadillas_anuncio.LoadTextureA();
	gorditas_anuncio = Texture("Textures/gorditas_anuncio.tga");
	gorditas_anuncio.LoadTextureA();
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
	Fuente = Model();
	Fuente.LoadModel("Models/fuenteAgua.obj");
	Tacos = Model();
	Tacos.LoadModel("Models/puestoTacos.obj");
	Trompo1 = Model();
	Trompo1.LoadModel("Models/Trompo1.obj");
	Trompo2 = Model();
	Trompo2.LoadModel("Models/Trompo2.obj");

	Fount1 = Texture("Textures/Fountain_BaseColor.png");
	Fount1.LoadTextureA();
	Fount2 = Texture("Textures/Fountain_Metallic.png");
	Fount2.LoadTextureA();
	Fount3 = Texture("Textures/Fountain_Normal.png");
	Fount3.LoadTextureA();
	Fount4 = Texture("Textures/Fountain_Roughness.png");
	Fount4.LoadTextureA();
	Concreto = Texture("Textures/concreto.tga");
	Concreto.LoadTextureA();
	Road = Texture("Textures/road.tga");
	Road.LoadTextureA();
	aguaFuente = Texture("Textures/aguaFount.tga");
	aguaFuente.LoadTextureA();
	Carne = Texture("Textures/meat.tga");
	Carne.LoadTextureA();
	MetalShine = Texture("Textures/metal_shine.tga");
	MetalShine.LoadTextureA();
	MetalR = Texture("Textures/metal_rough.tga");
	MetalR.LoadTextureA();
	MetalS = Texture("Textures/metal_shamed1.tga");
	MetalS.LoadTextureA();
	puestoTacos = Texture("Textures/puestoTacos.tga");
	puestoTacos.LoadTextureA();

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
	dayLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.7f, 0.7f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	
	nightLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.2f, 0.2f,
		0.0f, 0.0f, -1.0f);

	unsigned int spotLightCount = 0;
	//linterna
	linterna = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	rotllanta = 0.00f; //
	rotllantaOffset = 2.0f; //

	start = std::clock();

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		// ****************** CAMBIO DE DIA A NOCHE Y NOCHE A DIA ******************
		finish = std::clock();
		duration = (finish - start) / (double)CLOCKS_PER_SEC;
		
		/*if (duration < 10) { // MODIFICAR TIEMPO
			skybox_function(1);
			mainLight = dayLight;
		}else if (duration < 20) { // MODIFICAR TIEMPO
			skybox_function(0);
			mainLight = nightLight;
		}else {
			start = finish;
			duration = 0;
		}*/

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		
		//Recibir eventos del usuario
		glfwPollEvents();

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), 0.0f);
		
		
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

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::mat4 modelcar(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
		glm::vec2 toffset_textura = glm::vec2(0.0f, 0.0f);
		glm::vec2 toffset_texturaDos = glm::vec2(0.0f, 0.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh(); // 2

		//// ------------------ PANADERIA ------------------ 
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 100.0f));
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, mainWindow.getFoodTruckMovimiento())); //ANIMACION BASICA #1
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//panaderia.RenderModel();

		//// ------------------ PUESTO AGUAS ------------------ 
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(100.0f, 0.0f, 0.0f));
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//puesto_aguas.RenderModel();
		//// ******************* ANUNCIO *******************
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(100.0f, 9.0f, 4.7f));
		//model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//aguas_frescas_anuncio.UseTexture();
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[1]->RenderMesh(); //8
		//// ****************** VASO ******************
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(100.0f, 0.0f, 7.0f));
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//vaso.RenderModel();
		//// ****************** JARRA ******************
		//rotllanta += rotllantaOffset * deltaTime;
		//subiendo += 0.001;
		//
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(100.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, 40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::translate(model, glm::vec3(((sin(glm::radians(rotllanta))) * glm::radians(rotllanta)) * 0.01, subiendo, (- 1 + (cos(glm::radians(rotllanta)) * glm::radians(rotllanta))) * 0.01));
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
		//model = glm::translate(model, glm::vec3(mainWindow.getQuesadillaA(), 0.5f, 0.0f)); //ANIMACION BASICA #2
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

		// ------------------ Cesped ------------------
		model = glm::mat4(1.0);
		modelaux = glm::mat4 (1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();

		// ------------------ Camino ------------------
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(275.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(225.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(175.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(125.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(25.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-25.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-75.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-125.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-175.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-225.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Road.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		// ------------------ Fuente ------------------
		float Fsc = 8.0f;

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(Fsc, Fsc, Fsc));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Fuente.RenderModel();

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
		model = glm::translate(model, glm::vec3(0.0f, 43.0f, 0.0f));
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
		model = glm::translate(model, glm::vec3(0.0f, 43.0f, 0.0f));
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

		// ------------------ Puesto Tacos ------------------
		float Tsc = 2.0f;
		float TtrX = -80.0, TtrZ = -80.0f;

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(TtrX, -0.9f, TtrZ));
		model = glm::scale(model, glm::vec3(Tsc, Tsc, Tsc));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Tacos.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(TtrX, -0.9f, TtrZ));
		model = glm::scale(model, glm::vec3(Tsc, Tsc, Tsc));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Trompo1.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(TtrX, -0.9f, TtrZ));
		model = glm::scale(model, glm::vec3(Tsc, Tsc, Tsc));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Trompo2.RenderModel();


		glDisable(GL_BLEND);
		
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}