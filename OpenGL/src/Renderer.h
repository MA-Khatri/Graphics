#pragma once

#include "vertex_array.h"
#include "index_buffer.h"
#include "shader.h"
#include "gl_utils.h"
#include "object.h"

class Renderer
{
public:
	void Clear() const;

	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, const GLenum draw_mode = GL_TRIANGLES) const;
	void Draw(Object object);


};