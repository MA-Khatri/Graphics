#pragma once

#include "glm/glm.hpp"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"

struct Primitive
{
	VertexArray* va;
	VertexBuffer* vb;
	IndexBuffer* ib;
	GLenum draw_mode;
};

Primitive CreateGroundPlaneGrid(int xcount, int ycount, float xymax, 
	glm::vec4 xcolor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), 
	glm::vec4 ycolor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

Primitive CreateAxes(glm::vec4 xcolor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
	glm::vec4 ycolor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
	glm::vec4 zcolor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

Primitive CreateRing(unsigned int div = 32, glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

Primitive CreatePlane(float size = 1.0f);

Primitive CreateUVSphere(unsigned int div = 32);