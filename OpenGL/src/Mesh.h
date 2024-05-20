#pragma once

#include "glm/glm.hpp"

#include "vertex_array.h"
#include "vertex_buffer.h"
#include "vertex_buffer_layout.h"
#include "index_buffer.h"

struct Mesh
{
	VertexArray* va;
	VertexBuffer* vb;
	IndexBuffer* ib;
	GLenum draw_mode;
};

Mesh CreateGroundPlaneGrid(int xcount, int ycount, float xymax, 
	glm::vec4 xcolor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), 
	glm::vec4 ycolor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

Mesh CreateAxes(glm::vec4 xcolor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
	glm::vec4 ycolor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
	glm::vec4 zcolor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

Mesh CreateRing(unsigned int div = 32, glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

Mesh CreatePlane();

Mesh CreateCube();

Mesh CreateUVSphere(unsigned int div = 32);

Mesh LoadOBJ(const std::string& filepath);