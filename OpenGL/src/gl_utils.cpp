#include "gl_utils.h"
#include <iostream>

void GLClearError() // we make GL capital here to note that we wrote the function!
{
    while (glGetError() != GL_NO_ERROR); // we could just write !glGetError() since no error == 0
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}