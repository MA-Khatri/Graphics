#include "shader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "gl_utils.h"

Shader::Shader(const std::string& filepath, Shader::Mode shader_mode /* = BASIC */)
	: m_VertexPath(filepath), m_FragmentPath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(filepath);

	switch (shader_mode) {

	case BASIC:
		m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
		break;

	case TESSELLATE:
		m_RendererID = CreateShader(source.VertexSource, source.TCSSource, source.TESSource, source.FragmentSource);
		break;

	case GEOMETRY:
		m_RendererID = CreateShader(source.VertexSource, source.GeometrySource, source.FragmentSource);
		break;

	case TESSELLATE_GEOMETRY:
		m_RendererID = CreateShader(source.VertexSource, source.TCSSource, source.TESSource, source.GeometrySource, source.FragmentSource);
		break;
	}

}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
	: m_VertexPath(vertexPath), m_FragmentPath(fragmentPath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShaders(vertexPath, fragmentPath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
	GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform2f(const std::string& name, float v0, float v1)
{
	GLCall(glUniform2f(GetUniformLocation(name), v0, v1));
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1)
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;

	m_UniformLocationCache[name] = location;
	return location;
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath); // open the file specified by path

	enum class ShaderType
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1,
		GEOMETRY = 2,
		TCS = 3, // Tessellation control shader
		TES = 4, // Tessellation evaluation shader
	};

	std::string line;
	std::stringstream ss[5]; // upto 5 types of shaders 
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
			else if (line.find("geometry") != std::string::npos)
				type = ShaderType::GEOMETRY;
			else if (line.find("tcs") != std::string::npos)
				type = ShaderType::TCS;
			else if (line.find("tes") != std::string::npos)
				type = ShaderType::TES;
		}
		else // add it to the shader source code
		{
			ss[(int)type] << line << "\n";
		}
	}

	return { ss[0].str(), ss[1].str(), ss[2].str(), ss[3].str(), ss[4].str() };
}

ShaderProgramSource Shader::ParseShaders(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::ifstream v_stream(vertexPath);
	std::ifstream f_stream(fragmentPath);

	std::string line;
	std::stringstream ss[2]; // one for vertex, another for fragment shader
	
	while (getline(v_stream, line))
	{
		ss[0] << line << "\n";
	}

	while (getline(f_stream, line))
	{
		ss[1] << line << "\n";
	}

	return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) // type is actually a GLEnum* (which is an unsigned int)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str(); // get ptr to string data
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::string typeStr;
		switch (type)
		{
		case GL_VERTEX_SHADER:
			typeStr = "VERTEX";
			break;

		case GL_FRAGMENT_SHADER:
			typeStr = "FRAGMENT";
			break;

		case GL_GEOMETRY_SHADER:
			typeStr = "GEOMETRY";
			break;

		case GL_TESS_CONTROL_SHADER:
			typeStr = "TSC";
			break;

		case GL_TESS_EVALUATION_SHADER:
			typeStr = "TES";
			break;
		}
		std::cout << "[Shader Error] Failed to compile " << typeStr << " shader for file " << m_VertexPath << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

// We need to provide OpenGL with our shader source code and have it compile the shader and return an identifier 
// which we can use to bind the shader
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	GLCall(unsigned int program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	// Detach and clean up the intermediate programs
	// Might want to keep the shaders for debugging and profiling
	//GLCall(glDetachShader(program, vs));
	//GLCall(glDetachShader(program, fs));
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& geometryShader, const std::string& fragmentShader)
{
	GLCall(unsigned int program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, gs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	// Detach and clean up the intermediate programs
	// Might want to keep the shaders for debugging and profiling
	//GLCall(glDetachShader(program, vs));
	//GLCall(glDetachShader(program, gs));
	//GLCall(glDetachShader(program, fs));
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(gs));
	GLCall(glDeleteShader(fs));

	return program;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& tcsShader, const std::string& tesShader, const std::string& fragmentShader)
{
	GLCall(unsigned int program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int tcs = CompileShader(GL_TESS_CONTROL_SHADER, tcsShader);
	unsigned int tes = CompileShader(GL_TESS_EVALUATION_SHADER, tesShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, tcs));
	GLCall(glAttachShader(program, tes));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	// Detach and clean up the intermediate programs
	// Might want to keep the shaders for debugging and profiling
	//GLCall(glDetachShader(program, vs));
	//GLCall(glDetachShader(program, tcs));
	//GLCall(glDetachShader(program, tes));
	//GLCall(glDetachShader(program, fs));
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(tcs));
	GLCall(glDeleteShader(tes));
	GLCall(glDeleteShader(fs));

	return program;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& tcsShader, const std::string& tesShader, const std::string& geometryShader, const std::string& fragmentShader)
{
	GLCall(unsigned int program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int tcs = CompileShader(GL_TESS_CONTROL_SHADER, tcsShader);
	unsigned int tes = CompileShader(GL_TESS_EVALUATION_SHADER, tesShader);
	unsigned int gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, tcs));
	GLCall(glAttachShader(program, tes));
	GLCall(glAttachShader(program, gs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	// Detach and clean up the intermediate programs
	// Might want to keep the shaders for debugging and profiling
	//GLCall(glDetachShader(program, vs));
	//GLCall(glDetachShader(program, tcs));
	//GLCall(glDetachShader(program, tes));
	//GLCall(glDetachShader(program, gs));
	//GLCall(glDetachShader(program, fs));
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(tcs));
	GLCall(glDeleteShader(tes));
	GLCall(glDeleteShader(gs));
	GLCall(glDeleteShader(fs));

	return program;
}
