#include "light.h"

Light::Light(glm::vec3 position, glm::vec3 color)
	: position(position), color(color)
{
	UpdateMatrix();
}

Light::Light(glm::vec3 position)
	: position(position), color(glm::vec3(1.0f, 1.0f, 1.0f))
{
	UpdateMatrix();
}

Light::~Light()
{

}

void Light::UpdatePosition(glm::vec3 posn)
{
	position = posn;
	UpdateMatrix();
}

void Light::UpdateColor(glm::vec3 clr)
{
	color = clr;
}

void Light::UpdateMatrix()
{
	/* For now, setting the light matrix to look towards the origin */
	//view_matrix = glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
	view_matrix = glm::lookAt(position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
	projection_matrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);
	//projection_matrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.01f, 100.0f);

	matrix = projection_matrix * view_matrix;
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
