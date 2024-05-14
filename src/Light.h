#pragma once

#include <string>
#include "glm/glm.hpp"

#include "Shader.h"

class Light
{
public:
	Light(glm::vec3 position, glm::vec3 color);
	Light(glm::vec3 position);
	~Light();

	void UpdatePosition(glm::vec3 posn);
	void UpdateColor(glm::vec3 color);

	//void SetPositionUniform(Shader* shader, std::string& uniform_name);
	//void SetColorUniform(Shader* shader, std::string& uniform_name);

public:
	glm::vec3 position;
	glm::vec3 color;
};