#include "Texture.h"

#include "stb_image/stb_image.h"

Texture::Texture(const std::string& path, const std::string texture_type /*= "Diffuse*/, GLenum pixel_format /*= GL_RGBA*/, GLenum pixel_type /*= GL_UNSIGNED_BYTE*/,
	GLenum min_filter /*= GL_LINEAR*/, GLenum mag_filter /*= GL_LINEAR*/, GLenum wrap_s /*= GL_REPEAT*/, GLenum wrap_t /*= GL_REPEAT*/)
	: m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0), m_TextureType(texture_type)
{
	stbi_set_flip_vertically_on_load(1);
	if (pixel_format == GL_RED) m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 1);
	else m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mag_filter));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_t));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, pixel_format, pixel_type, m_LocalBuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0)); // unbind

	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot)); // slots are unsigned ints. They are also ordered numerically. 32 total.
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}