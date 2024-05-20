#include "compute.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

Compute::Compute(const std::string& filepath)
	: m_ShaderPath(filepath), m_RendererID(0)
{
	std::string source = ParseShader(filepath);
	m_RendererID = CreateShader(source);
}

Compute::~Compute()
{
	GLCall(glDeleteProgram(m_RendererID));
}

void Compute::Bind() const
{

}

void Compute::Unbind() const
{

}

std::string Compute::ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	std::string line;
	std::stringstream ss;

	while (getline(stream, line))
		ss << line << "\n";

	return ss.str();
}

unsigned int Compute::CompileShader(unsigned int type, const std::string& source)
{

}

unsigned int Compute::CreateShader(const std::string& shader)
{

}

int Compute::GetUniformLocation(const std::string& name)
{

}
