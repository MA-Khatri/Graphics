#pragma once

#include "glUtils.h"
#include "Shader.h"
#include <iostream>

class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
	std::string m_TextureType;
	GLuint m_PixelFormat;
	GLenum m_PixelType;

public:
	Texture(const std::string& path, const std::string texture_type = "Diffuse", GLenum pixel_format = GL_RGBA, GLenum pixel_type = GL_UNSIGNED_BYTE,
		GLenum min_filter = GL_LINEAR, GLenum mag_filter = GL_LINEAR, GLenum wrap_s = GL_REPEAT, GLenum wrap_t = GL_REPEAT);
	Texture(unsigned char* bytes, int width, int height, const std::string texture_type = "Diffuse", GLenum pixel_format = GL_RGB, GLenum pixel_type = GL_UNSIGNED_BYTE,
		GLenum min_filter = GL_NEAREST, GLenum mag_filter = GL_NEAREST, GLenum wrap_s = GL_CLAMP_TO_EDGE, GLenum wrap_t = GL_CLAMP_TO_EDGE);
	~Texture();

	void Update(); // Uses the predefined m_FilePath -- will print a warning if m_FilePath is empty
	void Update(unsigned char* bytes, int width, int height);

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	inline std::string GetType() const { return m_TextureType; }
	inline GLuint GetTexture() { return m_RendererID; }

private:
	void LoadImage(); // calls stbi_load on existing m_FilePath and updates m_LocalBuffer
	void CreateTexture(GLenum min_filter, GLenum mag_filter, GLenum wrap_s, GLenum wrap_t);
};