#include "object.h"

Object::Object(Mesh mesh)
	:va(mesh.va), vb(mesh.vb), ib(mesh.ib), draw_mode(mesh.draw_mode)
{

}

Object::Object(Mesh mesh, std::vector<Texture*> textures)
	:va(mesh.va), vb(mesh.vb), ib(mesh.ib), draw_mode(mesh.draw_mode), textures(textures)
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

void Object::UpdateNormalMatrix()
{
	model_normal_matrix = glm::transpose(glm::inverse(model_matrix));
}

void Object::SetTransform(glm::mat4x4 transform)
{
	model_matrix = transform;
	UpdateNormalMatrix();
}

void Object::UpdateTransform(glm::mat4x4 transform)
{
	model_matrix *= transform;
	UpdateNormalMatrix();
}

void Object::Translate(glm::vec3 translate)
{
	model_matrix *= glm::translate(translate);
	UpdateNormalMatrix();
}

void Object::Translate(float x, float y, float z)
{
	model_matrix *= glm::translate(glm::vec3(x, y, z));
	UpdateNormalMatrix();
}

void Object::Rotate(glm::vec3 axis, float deg)
{
	model_matrix *= glm::rotate(glm::radians(deg), axis);
	UpdateNormalMatrix();
}

void Object::Scale(float scale)
{
	model_matrix *= glm::scale(glm::vec3(scale, scale, scale));
	UpdateNormalMatrix();
}

void Object::Scale(glm::vec3 scale)
{
	model_matrix *= glm::scale(scale);
	UpdateNormalMatrix();
}

void Object::Scale(float x, float y, float z)
{
	model_matrix *= glm::scale(glm::vec3(x, y, z));
	UpdateNormalMatrix();
}

void Object::UpdateTextures(std::vector<Texture*> new_textures)
{
	textures = new_textures;
}

void Object::Draw()
{
	Shader default_shader = Shader(AbsPath("res/shaders/Default.shader"));
	default_shader.Bind();
	va->Bind();

	default_shader.SetUniformMat4f("u_MVP", model_matrix);
	default_shader.SetUniformMat4f("u_Model", model_matrix);
	default_shader.SetUniformMat4f("u_ModelNormal", model_normal_matrix);
	default_shader.SetUniform3f("u_LightPosition", 100.0, 100.0, -100.0);

	GLCall(glDrawElements(draw_mode, ib->GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Object::Draw(Camera camera)
{
	Shader default_shader = Shader(AbsPath("res/shaders/Default.shader"));
	default_shader.Bind();
	va->Bind();

	default_shader.SetUniformMat4f("u_MVP", camera.matrix * model_matrix);
	default_shader.SetUniformMat4f("u_Model", model_matrix);
	default_shader.SetUniformMat4f("u_ModelNormal", model_normal_matrix);
	default_shader.SetUniform3f("u_LightPosition", camera.position.x, camera.position.y, camera.position.z);

	GLCall(glDrawElements(draw_mode, ib->GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Object::Draw(Camera camera, Shader& shader, unsigned int uniforms_mode /* = 0 */)
{
	shader.Bind();
	va->Bind();

	// Keep track of how many of each type of textures we have
	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;
	unsigned int numCubeMap = 0;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		std::string num;
		std::string type = textures[i]->GetType();
		if (type == "Diffuse")
		{
			num = std::to_string(numDiffuse++);
			textures[i]->Bind(i);
		}
		else if (type == "Specular")
		{
			num = std::to_string(numSpecular++);
			textures[i]->Bind(i);
		}
		else if (type == "CubeMap")
		{
			num = std::to_string(numCubeMap++);
			textures[i]->Bind(i, GL_TEXTURE_CUBE_MAP);
		}
		shader.SetUniform1i(("u_" + type + num).c_str(), i);
	}

	if (uniforms_mode == 0)
	{
		shader.SetUniformMat4f("u_MVP", camera.matrix * model_matrix);
		shader.SetUniformMat4f("u_Model", model_matrix);
		shader.SetUniformMat4f("u_ModelNormal", model_normal_matrix);
		shader.SetUniform3f("u_LightPosition", camera.position.x, camera.position.y, camera.position.z);
	}
	else if (uniforms_mode == 1)
	{
		shader.SetUniformMat4f("u_MVP", camera.matrix * model_matrix);
		shader.SetUniformMat4f("u_VP", camera.matrix * glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	}
	
	GLCall(glDrawElements(draw_mode, ib->GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Object::Draw(Light light)
{
	Shader basic_shader = Shader(AbsPath("res/shaders/Basic.shader"));
	basic_shader.Bind();
	va->Bind();


	basic_shader.SetUniformMat4f("u_MVP", light.matrix * model_matrix);

	GLCall(glDrawElements(draw_mode, ib->GetCount(), GL_UNSIGNED_INT, nullptr));
}
