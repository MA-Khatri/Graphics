#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "gl_utils.h"

// External globals set in Application.cpp
extern unsigned int window_width;
extern unsigned int window_height;
//extern unsigned int viewport_width;
//extern unsigned int viewport_height;
extern float yfov;
extern int ui_mode;

// Resizes the glViewport when the GLFW window is resized
void glfw_framebuffer_size_callback(GLFWwindow* window, int newWidth, int newHeight);

// Handles FOV adjustment with the scroll wheel
void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Handles all user input to the window
void glfw_process_input(GLFWwindow* window);