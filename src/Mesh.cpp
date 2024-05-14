#include <vector>

#include "OBJ-Loader.h"

#include "Mesh.h"

#include <cmath>
#include <iostream>

const float pi = 3.14159265358979323846f;

Mesh CreateGroundPlaneGrid(int xcount, int ycount, float xymax, glm::vec4 xcolor, glm::vec4 ycolor)
{
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	float xgap = xymax / (xcount - 1);
	float ygap = xymax / (ycount - 1);

	for (int v = 0; v < 2 * xcount; v++)
	{
		if (v % 2 == 0)
		{
			vertices.push_back(-xymax + v * xgap);
			vertices.push_back(-xymax);
		}
		else
		{
			vertices.push_back(-xymax + (v - 1) * xgap);
			vertices.push_back(xymax);
		}

		vertices.push_back(xcolor.r);
		vertices.push_back(xcolor.g);
		vertices.push_back(xcolor.b);
		vertices.push_back(xcolor.a);

		indices.push_back(v);
	}

	for (int v = 0; v < 2 * ycount; v++)
	{
		if (v % 2 == 0)
		{
			vertices.push_back(-xymax);
			vertices.push_back(-xymax + v * ygap);
		}
		else
		{
			vertices.push_back(xymax);
			vertices.push_back(-xymax + (v - 1) * ygap);
		}
		vertices.push_back(ycolor.r);
		vertices.push_back(ycolor.g);
		vertices.push_back(ycolor.b);
		vertices.push_back(ycolor.a);

		indices.push_back(v + 2 * xcount);
	}

	VertexArray* va = new VertexArray();
	va->Bind();
	
	VertexBuffer* vb = new VertexBuffer(&vertices[0], vertices.size() * sizeof(float));
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

	return Mesh{ va, vb, ib, GL_LINES };
}

Mesh CreateAxes(glm::vec4 xcolor, glm::vec4 ycolor, glm::vec4 zcolor)
{
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	vertices.insert(vertices.end(),
		{
			0.0f, 0.0f, 0.0f,
			xcolor.r, xcolor.g, xcolor.b, xcolor.a,

			1.0f, 0.0f, 0.0f,
			xcolor.r, xcolor.g, xcolor.b, xcolor.a,
		});

	vertices.insert(vertices.end(),
		{
			0.0f, 0.0f, 0.0f,
			ycolor.r, ycolor.g, ycolor.b, ycolor.a,

			0.0f, 1.0f, 0.0f,
			ycolor.r, ycolor.g, ycolor.b, ycolor.a
		});

	vertices.insert(vertices.end(),
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

	VertexBuffer* vb = new VertexBuffer(&vertices[0], vertices.size() * sizeof(float));
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

	return Mesh{ va, vb, ib, GL_LINES };
}

Mesh CreateRing(unsigned int div, glm::vec4 color)
{
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	float dtheta = 2 * pi / div;
	unsigned int i = 0;
	for (float theta = 0; theta < 2 * pi; theta += dtheta)
	{
		vertices.push_back(std::cos(theta));
		vertices.push_back(std::sin(theta));

		vertices.push_back(color.r);
		vertices.push_back(color.g);
		vertices.push_back(color.b);
		vertices.push_back(color.a);

		indices.push_back(i);
		i++;
	}

	VertexArray* va = new VertexArray();
	va->Bind();

	VertexBuffer* vb = new VertexBuffer(&vertices[0], vertices.size() * sizeof(float));
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

	return Mesh{ va, vb, ib, GL_LINE_LOOP };
}

Mesh CreatePlane()
{
	float vertices[] = {
		 0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // 0
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // 1
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 2
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // 3
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	VertexArray* va = new VertexArray();
	va->Bind();

	VertexBuffer* vb = new VertexBuffer(vertices, 4 * 7 * sizeof(float));
	vb->Bind();
	VertexBufferLayout layout;
	layout.Push<float>(2); // xy posn
	layout.Push<float>(3); // xyz normal
	layout.Push<float>(2); // uv tex coord
	va->AddBuffer(*vb, layout);

	IndexBuffer* ib = new IndexBuffer(indices, 6);
	ib->Bind();

	va->Unbind();
	vb->Unbind();
	ib->Unbind();

	return Mesh{ va, vb, ib, GL_TRIANGLES };
}

Mesh CreateCube()
{ // from https://pastebin.com/XiCprv6S
	std::vector<float> vertices = {
	   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // A 0
	   0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // B 1
	   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // C 2
	   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // D 3
	   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // E 4
	   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   // F 5
	   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   // G 6
	   -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,   // H 7

	   -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,  // D 8
	   -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // A 9
	   -0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  // E 10
	   -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  // H 11
	   0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   // B 12
	   0.5f,  0.5f, -0.5f,  1.0f, 0.0f,   // C 13
	   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   // G 14
	   0.5f, -0.5f,  0.5f,  0.0f, 1.0f,   // F 15

	   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // A 16
	   0.5f, -0.5f, -0.5f,  1.0f, 0.0f,   // B 17
	   0.5f, -0.5f,  0.5f,  1.0f, 1.0f,   // F 18
	   -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  // E 19
	   0.5f,  0.5f, -0.5f,   0.0f, 0.0f,  // C 20
	   -0.5f,  0.5f, -0.5f,  1.0f, 0.0f,  // D 21
	   -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // H 22
	   0.5f,  0.5f,  0.5f,   0.0f, 1.0f,  // G 23
	};
	
	std::vector<unsigned int> indices = {
		// front and back
		0, 3, 2,
		2, 1, 0,
		4, 5, 6,
		6, 7 ,4,
		// left and right
		11, 8, 9,
		9, 10, 11,
		12, 13, 14,
		14, 15, 12,
		// bottom and top
		16, 17, 18,
		18, 19, 16,
		20, 21, 22,
		22, 23, 20
	};

	VertexArray* va = new VertexArray();
	va->Bind();

	VertexBuffer* vb = new VertexBuffer(&vertices[0], vertices.size() * sizeof(float));
	vb->Bind();
	VertexBufferLayout layout;
	layout.Push<float>(3); // xyz posn
	layout.Push<float>(2); // uv tex coord
	va->AddBuffer(*vb, layout);

	IndexBuffer* ib = new IndexBuffer(&indices[0], indices.size());
	ib->Bind();

	va->Unbind();
	vb->Unbind();
	ib->Unbind();

	return Mesh{ va, vb, ib, GL_TRIANGLES };
}

Mesh CreateUVSphere(unsigned int div)
{
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	for (unsigned int j = 0; j <= div; j++)
	{
		float aj = j * pi / div;
		float sj = std::sin(aj);
		float cj = std::cos(aj);

		for (unsigned int i = 0; i <= div; i++)
		{
			float ai = i * 2 * pi / div;
			float si = std::sin(ai);
			float ci = std::cos(ai);

			/* Positions */
			vertices.push_back(si * sj); // X
			vertices.push_back(cj);      // Y
			vertices.push_back(ci * sj); // Z

			/* Normals (which are the same) */
			vertices.push_back(si * sj); // X
			vertices.push_back(cj);      // Y
			vertices.push_back(ci * sj); // Z

			vertices.push_back(ai / (2.0f * pi)); // U
			vertices.push_back(aj / (2.0f * pi)); // V
		}
	}

	for (unsigned int j = 0; j < div; j++)
	{
		for (unsigned int i = 0; i < div; i++)
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

	VertexBuffer* vb = new VertexBuffer(&vertices[0], vertices.size() * sizeof(float));
	vb->Bind();
	VertexBufferLayout layout;
	layout.Push<float>(3); // xyz posn
	layout.Push<float>(3); // xyz normal
	layout.Push<float>(2); // uv tex coord
	va->AddBuffer(*vb, layout);

	IndexBuffer* ib = new IndexBuffer(&indices[0], indices.size());
	ib->Bind();

	va->Unbind();
	vb->Unbind();
	ib->Unbind();

	return Mesh{ va, vb, ib, GL_TRIANGLES };
}

Mesh LoadOBJ(const std::string& filepath)
{ /* https://github.com/Bly7/OBJ-Loader */
	objl::Loader Loader;

	bool loadout = Loader.LoadFile(filepath);

	if (!loadout)
	{
		std::cout << "[Mesh:LoadOBJ] ERROR! Failed to load OBJ file '" << filepath << "'" << std::endl;
		return { nullptr, nullptr, nullptr, NULL };
	}

	if (Loader.LoadedMeshes.size() > 1)
	{
		std::cout << "[Mesh:LoadOBJ] WARNING: More than 1 mesh found in file '" << filepath << "'. Loading only the first mesh!" << std::endl;
	}

	objl::Mesh mesh = Loader.LoadedMeshes[0];

	VertexArray* va = new VertexArray();
	va->Bind();

	VertexBuffer* vb = new VertexBuffer(&mesh.Vertices[0], mesh.Vertices.size() * 8 * sizeof(float));
	vb->Bind();
	VertexBufferLayout layout;
	layout.Push<float>(3); // xyz posn
	layout.Push<float>(3); // xyz normal
	layout.Push<float>(2); // uv tex coord
	va->AddBuffer(*vb, layout);

	IndexBuffer* ib = new IndexBuffer(&mesh.Indices[0], mesh.Indices.size());
	ib->Bind();

	va->Unbind();
	vb->Unbind();
	ib->Unbind();

	return Mesh{ va, vb, ib, GL_TRIANGLES };
}

