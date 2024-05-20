#pragma once

#include <vector>

#include "vertex_array.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "texture.h"
#include "camera.h"
#include "mesh.h"

#include "glm/glm.hpp"
#include "gl_utils.h"

class Object 
{
private:
	VertexArray* va;
	VertexBuffer* vb;
	IndexBuffer* ib;
	GLenum draw_mode;

	std::vector<Texture*> textures;

	glm::mat4x4 model_matrix = glm::mat4x4(1.0f);

public:
	Object(Mesh mesh);
	Object(Mesh mesh, std::vector<Texture*> textures);
	~Object();

	void SetTransform(glm::mat4x4 transform);
	void UpdateTransform(glm::mat4x4 transform);

	void Translate(glm::vec3 translate);
	void Translate(float x, float y, float z);

	//void Rotate(...);

	void Scale(float scale);
	void Scale(glm::vec3 scale);
	void Scale(float x, float y, float z);

	void Draw(); // Draw with default shader
	void Draw(Camera camera); // Draw with default shader
	void Draw(Camera camera, Shader& shader);
};