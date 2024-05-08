#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "glUtils.h"

// External globals set in Application.cpp
extern unsigned int width;
extern unsigned int height;
extern float yfov;

// Resizes the glViewport when the GLFW window is resized
void glfw_framebuffer_size_callback(GLFWwindow* window, int newWidth, int newHeight);

// Handles FOV adjustment with the scroll wheel
void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Handles all user input to the window
void glfw_process_input(GLFWwindow* window);