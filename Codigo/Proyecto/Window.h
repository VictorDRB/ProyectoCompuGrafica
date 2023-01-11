#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

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

	GLfloat getmovecarx() { return movecarx; }
	GLfloat getmovecarz() { return movecarz; }
	GLfloat getmoveHx() { return moveHx; }
	GLfloat getmoveHy() { return moveHy; }

	GLfloat getDirx1() { return Dirx1; }
	GLfloat getDirx2() { return Dirx2; }
	GLfloat getDirH() { return DirH; }

	GLfloat getmuevex() { return muevex; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	GLfloat movecarx = 0.0f, movecarz = -20.0f;
	GLfloat moveHx = 0.0f, moveHy = 3.0f;
	GLfloat Dirx1 = 0.0f;
	GLfloat Dirx2 = 0.0f;
	GLfloat DirH = 0.0f;

	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

