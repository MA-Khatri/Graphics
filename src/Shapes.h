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
};

Shape CreateGroundPlaneGrid(int xcount, int ycount, float xymax, 
	glm::vec3 xcolor = glm::vec3(0.5f, 0.5f, 0.5f), 
	glm::vec3 ycolor = glm::vec3(0.5f, 0.5f, 0.5f));

Shape CreatePlane(float size = 1.0f);

Shape CreateAxes(glm::vec3 xcolor = glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3 ycolor = glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3 zcolor = glm::vec3(0.0f, 0.0f, 1.0f));
