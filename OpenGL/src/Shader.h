#pragma once
#include <string>
#include <unordered_map>

#include "glm/glm.hpp"
#include "utils.h"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
	std::string GeometrySource;
	std::string TCSSource;
	std::string TESSource;
};

class Shader
{
public:
	enum Mode {
		BASIC, // vertex and fragment shaders only
		TESSELLATE, // vertex, fragment, and tessellation shaders (TCS and TES)
		GEOMETRY, // vertex, fragment, and geometry shader
		TESSELLATE_GEOMETRY, // vertex, fragment, tessellation, and geometry shaders
	};

private:
	std::string m_VertexPath;
	std::string m_FragmentPath;
	unsigned int m_RendererID;
	Mode m_Mode;

	// Caching for uniforms
	std::unordered_map<std::string, int> m_UniformLocationCache;

public:
	Shader(const std::string& filepath, Shader::Mode shader_mode = Mode::BASIC); // Support for vertex and fragment shader in one file
	Shader(const std::string& vertexPath, const std::string& fragmentPath); // or vertex and fragment shaders in separate files
	~Shader();

	void Bind() const;
	void Unbind() const;

	// Set uniforms
	void SetUniform1f(const std::string& name, float value);
	void SetUniform1i(const std::string& name, int value);
	void SetUniform2f(const std::string& name, float v0, float v1);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
	// TODO: more...

private:
	ShaderProgramSource ParseShader(const std::string& filepath);
	ShaderProgramSource ParseShaders(const std::string& vertexPath, const std::string& fragmentPath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& geometryShader, const std::string& fragmentShader);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& tcsShader, const std::string& tesShader, const std::string& fragmentShader);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& tcsShader, const std::string& tesShader, const std::string& geometryShader, const std::string& fragmentShader);


	int GetUniformLocation(const std::string& name);
};