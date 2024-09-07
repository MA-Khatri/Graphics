#pragma once

#include <string>

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>


#include "shader.h"

class Light
{
public:
	Light(glm::vec3 position, glm::vec3 color, int mode = DIRECTIONAL);
	Light(glm::vec3 position, int mode = 0);
	~Light();

	void UpdatePosition(glm::vec3 posn);
	void UpdateColor(glm::vec3 clr);
	void UpdateMode(int new_mode);
	void UpdateMatrix();

	//void SetPositionUniform(Shader* shader, std::string& uniform_name);
	//void SetColorUniform(Shader* shader, std::string& uniform_name);

public:
	glm::vec3 position;
	glm::vec3 color;
	glm::mat4 view_matrix = glm::mat4(1.0f);
	glm::mat4 projection_matrix = glm::mat4(1.0f);
	glm::mat4 matrix = glm::mat4(1.0f); // view-projection matrix of light used to compute shadow maps
	
	int mode; // light mode: 0 = directional, 1 = point, 2 = spot/cone

	static const int DIRECTIONAL = 0;
	static const int POINT = 1;
	static const int SPOT = 2;
};