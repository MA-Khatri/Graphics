#include "Object.h"

Object::Object(Mesh mesh)
	:va(mesh.va), vb(mesh.vb), ib(mesh.ib), draw_mode(mesh.draw_mode)
{

}

Object::~Object()
{
	va->Unbind();
	vb->Unbind();
	ib->Unbind();

	delete(va);
	delete(vb);
	delete(ib);
}

void Object::SetTransform(glm::mat4x4 transform)
{
	model_matrix = transform;
}

void Object::UpdateTransform(glm::mat4x4 transform)
{
	model_matrix *= transform;
}

void Object::Translate(glm::vec3 translate)
{
	model_matrix *= glm::translate(translate);
}

void Object::Translate(float x, float y, float z)
{
	model_matrix *= glm::translate(glm::vec3(x, y, z));
}

void Object::Scale(float scale)
{
	model_matrix *= glm::scale(glm::vec3(scale, scale, scale));
}

void Object::Scale(glm::vec3 scale)
{
	model_matrix *= glm::scale(scale);
}

void Object::Scale(float x, float y, float z)
{
	model_matrix *= glm::scale(glm::vec3(x, y, z));
}

void Object::Draw()
{
	Shader default_shader = Shader("res/shaders/Default.shader");
	default_shader.Bind();
	va->Bind();

	default_shader.SetUniformMat4f("u_MVP", model_matrix);

	GLCall(glDrawElements(draw_mode, ib->GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Object::Draw(Camera camera)
{
	Shader default_shader = Shader("res/shaders/Default.shader");
	default_shader.Bind();
	va->Bind();

	default_shader.SetUniformMat4f("u_MVP", camera.matrix * model_matrix);

	GLCall(glDrawElements(draw_mode, ib->GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Object::Draw(Camera camera, Shader& shader)
{
	shader.Bind();
	va->Bind();

	shader.SetUniformMat4f("u_MVP", camera.matrix * model_matrix);
	
	GLCall(glDrawElements(draw_mode, ib->GetCount(), GL_UNSIGNED_INT, nullptr));
}
