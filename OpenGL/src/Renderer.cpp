#include "Renderer.h"
#include <iostream>

void Renderer::Clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, const GLenum draw_mode) const
{
    shader.Bind();
    va.Bind();
    ib.Bind(); // technically not necessary since VAO already binds IBO

    GLCall(glDrawElements(draw_mode, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Draw(Object object)
{
    
}

