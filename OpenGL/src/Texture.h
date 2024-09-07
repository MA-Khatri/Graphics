#pragma once

#include "gl_utils.h"
#include "shader.h"
#include <iostream>

class Texture
{
public:
	/* Texture types */
	enum Type {
		DIFFUSE,
		SPECULAR,
		CUBEMAP,
		SHADOWMAP,
		NORMALMAP,
		DISPLACEMENTMAP,
	};

private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
	Texture::Type m_TextureType;
	GLuint m_PixelFormat;
	GLenum m_PixelType;

public:
	Texture(const std::string& path, Texture::Type texture_type = Type::DIFFUSE, GLenum pixel_format = GL_RGBA, GLenum pixel_type = GL_UNSIGNED_BYTE,
		GLenum min_filter = GL_LINEAR, GLenum mag_filter = GL_LINEAR, GLenum wrap_s = GL_REPEAT, GLenum wrap_t = GL_REPEAT);
	Texture(unsigned char* bytes, int width, int height, Texture::Type texture_type = Type::DIFFUSE, GLenum pixel_format = GL_RGB, GLenum pixel_type = GL_UNSIGNED_BYTE,
		GLenum min_filter = GL_NEAREST, GLenum mag_filter = GL_NEAREST, GLenum wrap_s = GL_CLAMP_TO_EDGE, GLenum wrap_t = GL_CLAMP_TO_EDGE);

	/* Internal textures, e.g., render to texture */
	Texture(GLuint textureID, int width, int height, Texture::Type texture_type = Type::DIFFUSE) : m_RendererID(textureID), m_TextureType(texture_type), m_Width(width), m_Height(height) {}; 
	
	/* Load cubemap texture by providing the path to the folder containing the cubemap images */
	Texture(const std::string& cube_map_folder_path, int width, const std::string& image_extension = ".png");
	
	~Texture();

	void Update(); /* Uses the predefined m_FilePath -- will print a warning if m_FilePath is empty */ 
	void Update(unsigned char* bytes, int width, int height); /* Used only for updating ray-traced image texture */

	void Bind(unsigned int slot = 0, GLenum texture_mode = GL_TEXTURE_2D) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	inline Texture::Type GetType() const { return m_TextureType; }
	inline GLuint GetTexture() { return m_RendererID; }

private:
	void LoadImage(); // calls stbi_load on existing m_FilePath and updates m_LocalBuffer
	void CreateTexture(GLenum min_filter, GLenum mag_filter, GLenum wrap_s, GLenum wrap_t);
};