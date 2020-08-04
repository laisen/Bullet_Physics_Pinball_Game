#pragma once
#include "GLCommon.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void error_callback(int error, const char* description);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);