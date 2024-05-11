#include <vector>

#include "Shapes.h"

Shape CreateGroundPlaneGrid(int xcount, int ycount, float xymax, glm::vec3 xcolor, glm::vec3 ycolor)
{
	std::vector<float> verts;
	std::vector<unsigned int> indices;

	float xgap = xymax / (xcount - 1);
	float ygap = xymax / (ycount - 1);

	for (int v = 0; v < 2 * xcount; v++)
	{
		if (v % 2 == 0)
		{
			verts.push_back(-xymax + v * xgap);
			verts.push_back(-xymax);
		}
		else
		{
			verts.push_back(-xymax + (v - 1) * xgap);
			verts.push_back(xymax);
		}
		verts.push_back(xcolor.r);
		verts.push_back(xcolor.g);
		verts.push_back(xcolor.b);

		indices.push_back(v);
	}

	for (int v = 0; v < 2 * ycount; v++)
	{
		if (v % 2 == 0)
		{
			verts.push_back(-xymax);
			verts.push_back(-xymax + v * ygap);
		}
		else
		{
			verts.push_back(xymax);
			verts.push_back(-xymax + (v - 1) * ygap);
		}
		verts.push_back(ycolor.r);
		verts.push_back(ycolor.g);
		verts.push_back(ycolor.b);

		indices.push_back(v + 2 * xcount);
	}

	VertexArray* va = new VertexArray();
	va->Bind();
	
	VertexBuffer* vb = new VertexBuffer(&verts[0], verts.size()*sizeof(float));
	vb->Bind();
	VertexBufferLayout layout;
	layout.Push<float>(2); // xy posn
	layout.Push<float>(3); // rgb color
	va->AddBuffer(*vb, layout);

	IndexBuffer* ib = new IndexBuffer(&indices[0], indices.size()*sizeof(unsigned int));
	ib->Bind();

	va->Unbind();
	vb->Unbind();
	ib->Unbind();

	return Shape{ va, vb, ib };
}

Shape CreateAxes(glm::vec3 xcolor, glm::vec3 ycolor, glm::vec3 zcolor)
{
	std::vector<float> verts;
	std::vector<unsigned int> indices;

	verts.insert(verts.end(),
		{
			0.0f, 0.0f, 0.0f,
			xcolor.r, xcolor.g, xcolor.b,

			1.0f, 0.0f, 0.0f,
			xcolor.r, xcolor.g, xcolor.b
		});

	verts.insert(verts.end(),
		{
			0.0f, 0.0f, 0.0f,
			ycolor.r, ycolor.g, ycolor.b,

			0.0f, 1.0f, 0.0f,
			ycolor.r, ycolor.g, ycolor.b
		});

	verts.insert(verts.end(),
		{
			0.0f, 0.0f, 0.0f,
			zcolor.r, zcolor.g, zcolor.b,

			0.0f, 0.0f, 1.0f,
			zcolor.r, zcolor.g, zcolor.b
		});

	for (int i = 0; i < 6; i++)
	{
		indices.push_back(i);
	}

	VertexArray* va = new VertexArray();
	va->Bind();

	VertexBuffer* vb = new VertexBuffer(&verts[0], verts.size() * sizeof(float));
	vb->Bind();
	VertexBufferLayout layout;
	layout.Push<float>(3); // xyz posn
	layout.Push<float>(3); // rgb color
	va->AddBuffer(*vb, layout);

	IndexBuffer* ib = new IndexBuffer(&indices[0], indices.size() * sizeof(unsigned int));
	ib->Bind();

	va->Unbind();
	vb->Unbind();
	ib->Unbind();

	return Shape{ va, vb, ib };
}

Shape CreatePlane(float size)
{
	float positions[] = {
		 size,  size, 1.0f, 1.0f, // 0
		 size, -size, 1.0f, 0.0f, // 1
		-size, -size, 0.0f, 0.0f, // 2
		-size,  size, 0.0f, 1.0f, // 3
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	VertexArray* va = new VertexArray();
	va->Bind();

	VertexBuffer* vb = new VertexBuffer(positions, 4 * 4 * sizeof(float));
	vb->Bind();
	VertexBufferLayout layout;
	layout.Push<float>(2);
	layout.Push<float>(2);
	va->AddBuffer(*vb, layout);

	IndexBuffer* ib = new IndexBuffer(indices, 6);
	ib->Bind();

	va->Unbind();
	vb->Unbind();
	ib->Unbind();

	return Shape{ va, vb, ib };
}