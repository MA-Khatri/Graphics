#include "texture.h"

#include "stb_image/stb_image.h"

Texture::Texture(const std::string& path, const std::string texture_type /*= "Diffuse*/, GLenum pixel_format /*= GL_RGBA*/, GLenum pixel_type /*= GL_UNSIGNED_BYTE*/,
	GLenum min_filter /*= GL_LINEAR*/, GLenum mag_filter /*= GL_LINEAR*/, GLenum wrap_s /*= GL_REPEAT*/, GLenum wrap_t /*= GL_REPEAT*/)
	: m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0), m_TextureType(texture_type), m_PixelFormat(pixel_format), m_PixelType(pixel_type)
{
	LoadImage();

	CreateTexture(min_filter, mag_filter, wrap_s, wrap_t);

	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);
}

Texture::Texture(unsigned char* bytes, int width, int height, const std::string texture_type /*= "Diffuse"*/, GLenum pixel_format /*= GL_RGB*/, GLenum pixel_type /*= GL_UNSIGNED_BYTE*/, 
	GLenum min_filter /*= GL_NEAREST*/, GLenum mag_filter /*= GL_NEAEREST*/, GLenum wrap_s /*= GL_CLAMP_TO_EDGE*/, GLenum wrap_t /*= GL_CLAMP_TO_EDGE*/)
	: m_RendererID(0), m_LocalBuffer(bytes), m_Width(width), m_Height(height), m_BPP(0), m_TextureType(texture_type), m_PixelFormat(pixel_format), m_PixelType(pixel_type)
{
	CreateTexture(min_filter, mag_filter, wrap_s, wrap_t);

	/* !! WARNING: m_LocalBuffer is not freed !! */
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Update()
{
	if (!m_FilePath.empty())
	{
		LoadImage();
		GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, m_PixelFormat, m_PixelType, m_LocalBuffer));
		GLCall(glBindTexture(GL_TEXTURE_2D, 0)); // unbind

		if (m_LocalBuffer)
			stbi_image_free(m_LocalBuffer);
	}
	else 
	{
		std::cerr << "WARNING: Texture update was called without an existing filepath!" << std::endl;
	}
}

void Texture::Update(unsigned char* bytes, int width, int height)
{
	m_Width = width;
	m_Height = height;
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, m_PixelFormat, m_PixelType, bytes));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0)); // unbind
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

void Texture::LoadImage()
{
	stbi_set_flip_vertically_on_load(1);
	if (m_PixelFormat == GL_RED) m_LocalBuffer = stbi_load(m_FilePath.c_str(), &m_Width, &m_Height, &m_BPP, 1);
	else if (m_PixelFormat == GL_RGB) m_LocalBuffer = stbi_load(m_FilePath.c_str(), &m_Width, &m_Height, &m_BPP, 3);
	else m_LocalBuffer = stbi_load(m_FilePath.c_str(), &m_Width, &m_Height, &m_BPP, 4);
}

void Texture::CreateTexture(GLenum min_filter, GLenum mag_filter, GLenum wrap_s, GLenum wrap_t)
{
	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mag_filter));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_t));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, m_PixelFormat, m_PixelType, m_LocalBuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0)); // unbind
}
