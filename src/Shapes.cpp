#include <vector>

#include "Shapes.h"

#include <cmath>

const float pi = 3.14159265358979323846f;

Shape CreateGroundPlaneGrid(int xcount, int ycount, float xymax, glm::vec4 xcolor, glm::vec4 ycolor)
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
		verts.push_back(xcolor.a);

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
		verts.push_back(ycolor.a);

		indices.push_back(v + 2 * xcount);
	}

	VertexArray* va = new VertexArray();
	va->Bind();
	
	VertexBuffer* vb = new VertexBuffer(&verts[0], verts.size() * sizeof(float));
	vb->Bind();
	VertexBufferLayout layout;
	layout.Push<float>(2); // xy posn
	layout.Push<float>(4); // rgba color
	va->AddBuffer(*vb, layout);

	IndexBuffer* ib = new IndexBuffer(&indices[0], indices.size());
	ib->Bind();

	va->Unbind();
	vb->Unbind();
	ib->Unbind();

	return Shape{ va, vb, ib, GL_LINES };
}

Shape CreateAxes(glm::vec4 xcolor, glm::vec4 ycolor, glm::vec4 zcolor)
{
	std::vector<float> verts;
	std::vector<unsigned int> indices;

	verts.insert(verts.end(),
		{
			0.0f, 0.0f, 0.0f,
			xcolor.r, xcolor.g, xcolor.b, xcolor.a,

			1.0f, 0.0f, 0.0f,
			xcolor.r, xcolor.g, xcolor.b, xcolor.a,
		});

	verts.insert(verts.end(),
		{
			0.0f, 0.0f, 0.0f,
			ycolor.r, ycolor.g, ycolor.b, ycolor.a,

			0.0f, 1.0f, 0.0f,
			ycolor.r, ycolor.g, ycolor.b, ycolor.a
		});

	verts.insert(verts.end(),
		{
			0.0f, 0.0f, 0.0f,
			zcolor.r, zcolor.g, zcolor.b, zcolor.a,

			0.0f, 0.0f, 1.0f,
			zcolor.r, zcolor.g, zcolor.b, zcolor.a
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
	layout.Push<float>(4); // rgba color
	va->AddBuffer(*vb, layout);

	IndexBuffer* ib = new IndexBuffer(&indices[0], indices.size());
	ib->Bind();

	va->Unbind();
	vb->Unbind();
	ib->Unbind();

	return Shape{ va, vb, ib, GL_LINES };
}

Shape CreateRing(unsigned int div, glm::vec4 color)
{
	std::vector<float> verts;
	std::vector<unsigned int> indices;

	float dtheta = 2 * pi / div;
	unsigned int i = 0;
	for (float theta = 0; theta < 2 * pi - dtheta; theta += dtheta)
	{
		verts.push_back(std::cos(theta));
		verts.push_back(std::sin(theta));

		verts.push_back(color.r);
		verts.push_back(color.g);
		verts.push_back(color.b);
		verts.push_back(color.a);

		indices.push_back(i);
		i++;
	}

	VertexArray* va = new VertexArray();
	va->Bind();

	VertexBuffer* vb = new VertexBuffer(&verts[0], verts.size() * sizeof(float));
	vb->Bind();
	VertexBufferLayout layout;
	layout.Push<float>(2); // xy posn
	layout.Push<float>(4); // rgba color
	va->AddBuffer(*vb, layout);

	IndexBuffer* ib = new IndexBuffer(&indices[0], indices.size());
	ib->Bind();

	va->Unbind();
	vb->Unbind();
	ib->Unbind();

	return Shape{ va, vb, ib, GL_LINE_LOOP };
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
	layout.Push<float>(2); // xy posn
	layout.Push<float>(2); // uv tex coord
	va->AddBuffer(*vb, layout);

	IndexBuffer* ib = new IndexBuffer(indices, 6);
	ib->Bind();

	va->Unbind();
	vb->Unbind();
	ib->Unbind();

	return Shape{ va, vb, ib, GL_TRIANGLES };
}

Shape CreateUVSphere(unsigned int div)
{
	std::vector<float> verts;
	std::vector<unsigned int> indices;

	for (int j = 0; j <= div; j++)
	{
		float aj = j * pi / div;
		float sj = std::sin(aj);
		float cj = std::cos(aj);

		for (int i = 0; i <= div; i++)
		{
			float ai = i * 2 * pi / div;
			float si = std::sin(ai);
			float ci = std::cos(ai);

			/* Positions */
			verts.push_back(si * sj); // X
			verts.push_back(cj);      // Y
			verts.push_back(ci * sj); // Z

			/* Normals (which are the same) */
			verts.push_back(si * sj); // X
			verts.push_back(cj);      // Y
			verts.push_back(ci * sj); // Z
		}
	}

	for (int j = 0; j < div; j++)
	{
		for (int i = 0; i < div; i++)
		{
			unsigned int p1 = j * (div + 1) + i;
			unsigned int p2 = p1 + (div + 1);

			indices.push_back(p1);
			indices.push_back(p2);
			indices.push_back(p1 + 1);

			indices.push_back(p1 + 1);
			indices.push_back(p2);
			indices.push_back(p2 + 1);
		}
	}

	VertexArray* va = new VertexArray();
	va->Bind();

	VertexBuffer* vb = new VertexBuffer(&verts[0], verts.size() * sizeof(float));
	vb->Bind();
	VertexBufferLayout layout;
	layout.Push<float>(3); // xyz posn
	layout.Push<float>(3); // xyz normal
	va->AddBuffer(*vb, layout);

	IndexBuffer* ib = new IndexBuffer(&indices[0], indices.size());
	ib->Bind();

	va->Unbind();
	vb->Unbind();
	ib->Unbind();

	return Shape{ va, vb, ib, GL_TRIANGLES };
}