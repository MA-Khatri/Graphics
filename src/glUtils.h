#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define ASSERT(x) if (!(x)) __debugbreak(); // the __ indicates that this is a MSVC compiler function
#define GLCall(x) GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);