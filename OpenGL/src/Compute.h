#pragma once

#include "gl_utils.h"
#include <string>

class Compute
{
private:
	std::string m_ShaderPath;
	unsigned int m_RendererID;

public:
	Compute(const std::string& filepath);
	~Compute();

	void Bind() const;
	void Unbind() const;

private:
	std::string ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& shader);
	int GetUniformLocation(const std::string& name);
};