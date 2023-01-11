/*
Semestre 2023-1
Práctica 8: Iluminación 2 
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION
//componente ambiental - la luz que recibe el objeto
//componente difusa - glcolor con uniform, color de textura
//componente specular - brillo, depende del angulo de obeservacion, ventor de incidencia y vector reflectante
//componente combined es la combinacion de todos
//Kd coeficiente difuso, Ks coeficiente especular
//en sl "Shader_light_frag" se declaran las luces, finalcolor*

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include<iostream>

#include <chrono>
#include <time.h>
#include <windows.h>
using namespace std;

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

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;


Model Kitt_M;
Model Llanta_M;
Model Camino_M;
Model Blackhawk_M;
Model Dado_M;

Model CarObj_M;
Model CofreObj_M;
Model Rueda01_M;
Model Rueda02_M;
Model Rueda03_M;
Model Rueda04_M;

//Auto Futurista
Model CarFObj_M;
Model GlassObj_M;
Model BWheel01_M;
Model BWheel02_M;
Model Wheel01_M;
Model Wheel02_M;

//Modelos P7
Model FarolaObj;
Model JLanternObj;

//Modelos P8
Model ExtC;
Model IntC;
Model Circ;

Skybox skybox;

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

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

int offCont = 100; 



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

	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


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

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.3f);

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
	Llanta_M.LoadModel("Models/k_rueda.3ds");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Camino_M = Model();
	Camino_M.LoadModel("Models/railroad track.obj");

	CarObj_M = Model();
	CarObj_M.LoadModel("MP5/CarObj_Select.obj");
	CofreObj_M = Model();
	CofreObj_M.LoadModel("MP5/CofreObj_Select.obj");
	Rueda01_M = Model();
	Rueda01_M.LoadModel("MP5/Rueda01Obj_Select.obj");
	Rueda02_M = Model();
	Rueda02_M.LoadModel("MP5/Rueda02Obj_Select.obj");
	Rueda03_M = Model();
	Rueda03_M.LoadModel("MP5/Rueda03Obj_Select.obj");
	Rueda04_M = Model();
	Rueda04_M.LoadModel("MP5/Rueda04Obj_Select.obj");

	CarFObj_M = Model();
	CarFObj_M.LoadModel("MFC/CarfObj.obj");
	GlassObj_M = Model();
	GlassObj_M.LoadModel("MFC/GlassObj.obj");
	BWheel01_M = Model();
	BWheel01_M.LoadModel("MFC/BackWheelObj01.obj");
	BWheel02_M = Model();
	BWheel02_M.LoadModel("MFC/BackWheelObj02.obj");
	Wheel01_M = Model();
	Wheel01_M.LoadModel("MFC/Wheel01Obj.obj");
	Wheel02_M = Model();
	Wheel02_M.LoadModel("MFC/Wheel02Obj.obj");

//################### Practica 7 - Models ######################
	FarolaObj = Model();
	FarolaObj.LoadModel("MP7/farola.obj");
	JLanternObj = Model();
	JLanternObj.LoadModel("MP7/Japanese_lantern.obj");

//################### Practica 8 - Models ######################
	ExtC = Model();
	ExtC.LoadModel("MP8/pistaExtObj.obj");
	IntC = Model();
	IntC.LoadModel("MP8/pistaIntObj.obj");
	Circ = Model();
	Circ.LoadModel("MP8/circuito2Obj.obj");

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
		0.3f, 0.3f,//pa modificar el sol
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;


	//Declaración de primer luz puntual
	pointLights[0] = PointLight(0.95f, 0.28f, 0.07f, //color
		1.0f, 2.0f,//aIntensity, dIntensity ; luz que ilumina alrrededor, tiene mas peso la ambiental que la difusa
	   -15.0f, 5.0f, -30.0f,
		1.0f, 0.09f, 0.032f);//ec cuadratica
	pointLightCount++;

	//unsigned int spotLightCount = 0;

	pointLights[1] = PointLight(0.8f, 0.0f, 0.0f, //color
		1.0f, 2.0f,//aIntensity, dIntensity ; luz que ilumina alrrededor, tiene mas peso la ambiental que la difusa
		-35.0f, 0.5f, -30.0f,
		1.0f, 0.22f, 0.20f);//ec cuadratica
	pointLightCount++;

	unsigned int spotLightCount = 0;
	
	
	//linterna
	spotLights[0] = SpotLight(0.0f, 1.0f, 1.0f,//color CIAN
		 1.0f,   2.0f, //aIntensity, dIntensity
		 5.0f,   10.0f,  0.0f, //vector posicion
		-10.0f, -3.0f,  -1.0f, //vector direcccion
		 1.0f,   0.022f, 0.0019f, //ec de seg grado - cuadratica
		 15.0f); //edge
	spotLightCount++;

	
	//luz fija 1
	spotLights[1] = SpotLight(1.0f, 0.0f, 1.0f, //color MAGENTA
		 1.0f, 2.0f, //aIntensity, dIntensity
		 -55.0f, 10.0f, 0.0f,//vector posicion
		-10.0f, -3.0f, -1.0f,//vector direcccion
		 1.0f, 0.022f, 0.0019f,//ec de seg grado
		 15.0f); //edge
	spotLightCount++;

	//luz fija 2
	spotLights[2] = SpotLight(1.0f, 1.0f, 0.0f, //color AMARILLO
		 1.0f, 2.0f, //aIntensity, dIntensity
		 5.0f, 10.0f, 0.0f,//vector posicion
		-10.0f, -3.0f, -1.0f,//vector direcccion
		 1.0f, 0.022f, 0.0019f,//ec de seg grado
		 15.0f);
	spotLightCount++;


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

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

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);
		
	//********* Segmento de codigo que permite el apagado y encendido de la luz de Helicoptero en intervalos ***
		if (offCont > 50 && offCont < 101)
		{
			shaderList[0].SetSpotLights(spotLights, spotLightCount - 1);
			offCont--;
		}
		else if (offCont == 50) {
			offCont = 0;
		}
		else
		{
			shaderList[0].SetSpotLights(spotLights, spotLightCount);
			offCont++;
			if (offCont == 50)
			{
				offCont = 100;
			}
		}
	//************************************************************************

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();


		//################################## MyCar #################################################################

		glm::vec3 lPos = glm::vec3(4.0f, 0.0f, -21.0f);//Posicion de la luz para el SetFlash
		glm::vec3 lDir = glm::vec3(-1.0f, 0.0f, 0.0f);//Direccion de la luz para el SetFlash

		// luz ligada al auto
		lPos.x += mainWindow.getmovecarx();
		lDir.x = mainWindow.getDirx1();
		spotLights[0].SetFlash(lPos,lDir);

		lDir.x = mainWindow.getDirx2();
		spotLights[1].SetFlash(lPos, lDir);

		model = glm::mat4(1.0);
		color = glm::vec3(1.0f, 0.85f, 0.2f);
		//model = glm::translate(model, glm::vec3(-15.0f, 0.5f, -20.0f));
		model = glm::translate(model, glm::vec3(mainWindow.getmovecarx(), 0.5f, mainWindow.getmovecarz()));
		modelaux = model;
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //rotacion 90°
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CarFObj_M.RenderModel();

		//####### Glass ############
		model = modelaux;
		color = glm::vec3(0.0f, 0.8f, 0.8f);
		model = glm::translate(model, glm::vec3(-0.1f, -0.1f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.8f, 0.0f)); //rotacion 90°
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GlassObj_M.RenderModel();

		//####### Rueda01 ############
		model = modelaux;
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		model = glm::translate(model, glm::vec3(-2.0f, 0.5f, -0.3f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, .0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Wheel01_M.RenderModel();

		//####### Rueda02 ############
		model = modelaux;
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		model = glm::translate(model, glm::vec3(-2.0f, 0.5f, 0.3f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, .0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Wheel02_M.RenderModel();


		//####### Rueda03 ############
		model = modelaux;
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		model = glm::translate(model, glm::vec3(0.7f, 0.3f, -0.7f));
		model = glm::scale(model, glm::vec3(1.7f, 1.7f, 1.7f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, .0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BWheel01_M.RenderModel();

		//####### Rueda04 ############
		model = modelaux;
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		model = glm::translate(model, glm::vec3(0.7f, 0.3f, 0.7f));
		model = glm::scale(model, glm::vec3(1.7f, 1.7f, 1.7f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, .0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BWheel02_M.RenderModel();


	//################################## Helicopter #################################################################

		// luz ligada al helicoptero
		/*glm::vec3 HPos = glm::vec3(-1.0f, 0.0f, -1.0f);
		glm::vec3 HDir = glm::vec3(-1.0f, 0.0f,  0.0f);
		HPos.x += mainWindow.getmoveHx();
		HDir.x  = mainWindow.getDirH();
		spotLights[2].SetFlash(HPos, HDir);
		*/


		// luz ligada al helicoptero
		glm::vec3 HPos = glm::vec3(-1.0f, 0.0f, -1.0f);
		HPos.x += mainWindow.getmoveHx();
		spotLights[2].SetPos(HPos);


		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, 3.0f, -1.0));
		model = glm::translate(model, glm::vec3(mainWindow.getmoveHx(), mainWindow.getmoveHy(), -1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model,  90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();


	//################################## Farola #################################################################

		model = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::translate(model, glm::vec3(-15.0f, 0.5f, -30.0f));
		model = glm::scale(model, glm::vec3(3.5f, 3.5f, 3.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //rotacion 90°
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		FarolaObj.RenderModel();

	//################################## Linterna Japonesa #################################################################

		model = glm::mat4(1.0);
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		model = glm::translate(model, glm::vec3(-35.0f, 0.5f, -30.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //rotacion 90°
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JLanternObj.RenderModel();

	//################################## Race Circuit #################################################################
		//Circuito Exterior
		model = glm::mat4(1.0);
		color = glm::vec3(0.3f, 1.0f, 0.0f);
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 100.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ExtC.RenderModel();

		//Circuito Interior
		model = glm::mat4(1.0);
		color = glm::vec3(0.3f, 1.0f, 0.0f);
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 100.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		IntC.RenderModel();

		//Circuito Interior
		model = glm::mat4(1.0);
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.9f, 100.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Circ.RenderModel();


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
