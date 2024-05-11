#pragma once

#include "glm/glm.hpp"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"

struct Shape
{
	VertexArray* va;
	VertexBuffer* vb;
	IndexBuffer* ib;
	unsigned int mode;
};

/* Drawn with GL_LINES */
Shape CreateGroundPlaneGrid(int xcount, int ycount, float xymax, 
	glm::vec4 xcolor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), 
	glm::vec4 ycolor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

/* Drawn with GL_LINES */
Shape CreateAxes(glm::vec4 xcolor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
	glm::vec4 ycolor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
	glm::vec4 zcolor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

Shape CreateRing(unsigned int div = 32, glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

Shape CreatePlane(float size = 1.0f);

Shape CreateUVSphere(unsigned int div = 32);