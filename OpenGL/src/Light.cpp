#include "Light.h"

Light::Light(glm::vec3 position, glm::vec3 color)
	: position(position), color(color)
{

}

Light::Light(glm::vec3 position)
	: position(position), color(glm::vec3(1.0f, 1.0f, 1.0f))
{

}

Light::~Light()
{

}

void Light::UpdatePosition(glm::vec3 position)
{
	position = position;
}

void Light::UpdateColor(glm::vec3 color)
{
	color = color;
}

//void Light::SetPositionUniform(Shader* shader, std::string& uniform_name)
//{
//	shader->SetUniform3f(uniform_name, position.x, position.y, position.z);
//}
//
//void Light::SetColorUniform(Shader* shader, std::string& uniform_name)
//{
//	shader->SetUniform3f(uniform_name, color.x, color.y, color.z);
//}
