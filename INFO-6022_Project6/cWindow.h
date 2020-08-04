#pragma once
#include "GLCommon.h"

class cWindow
{
public:
	cWindow();
	~cWindow();

	int windowWidth;
	int windowHeight;
	const char *windowTitle;
	
	GLFWwindow* window;

	bool createWindow();
};

