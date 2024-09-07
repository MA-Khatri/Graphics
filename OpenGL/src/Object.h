#pragma once

#include <vector>

#include "vertex_array.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "texture.h"
#include "camera.h"
#include "light.h"
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
	Texture* rendered_texture;

	glm::mat4x4 model_matrix = glm::mat4x4(1.0f);
	glm::mat4x4 model_normal_matrix = glm::mat4x4(1.0f);

public:
	Object(Mesh mesh);
	Object(Mesh mesh, std::vector<Texture*> textures);
	~Object();

	void UpdateTextures(std::vector<Texture*> textures);

	void UpdateNormalMatrix();

	void SetTransform(glm::mat4x4 transform);
	void UpdateTransform(glm::mat4x4 transform);

	void Translate(glm::vec3 translate);
	void Translate(float x, float y, float z);

	void Rotate(glm::vec3 axis, float deg);

	void Scale(float scale);
	void Scale(glm::vec3 scale);
	void Scale(float x, float y, float z);

	glm::mat4x4 GetTransform() { return model_matrix; }
	glm::mat4x4 GetNormalTransform() { return model_normal_matrix; }
	void UpdateDrawMode(GLuint mode) { draw_mode = mode; }

	void Draw(); // Draw with default shader
	void Draw(Camera camera); // Draw with default shader
	void Draw(glm::mat4x4 matrix, Shader& shader, unsigned int uniforms_mode = 0);
	void Draw(Camera camera, Shader& shader, unsigned int uniforms_mode = 0);
	void Draw(Light light); // Draw the shadow map of the provided light
	void Draw(Light light, Shader& shader, unsigned int uniforms_mode = 0); // Draw the shadow map of the provided light with a custom shader...
};