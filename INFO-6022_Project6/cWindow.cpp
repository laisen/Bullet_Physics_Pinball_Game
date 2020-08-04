#include "cWindow.h"
#include <iostream>
#include "GFLW_callbacks.h"
#include "cCamera.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

cWindow::cWindow()
{
	this->windowWidth = 1080;
	this->windowHeight = 1080;
	this->windowTitle = "INFO-6022_Project6";
	std::cout << "A cWindow() is created." << std::endl;
	return;
}

cWindow::~cWindow()
{
	std::cout << "A cWindow() is destroyed." << std::endl;
	return;
}

bool cWindow::createWindow()
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSwapInterval(1);

	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);

	glClearColor(0.382f, 0.382f, 0.382f, 1.0f);

	return true;
}