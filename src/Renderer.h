#pragma once

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "glUtils.h"
#include "Object.h"

class Renderer
{
public:
	void Clear() const;

	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, const GLenum draw_mode = GL_TRIANGLES) const;
	void Draw(Object object);


};