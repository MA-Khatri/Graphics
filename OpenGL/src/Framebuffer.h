#pragma once

#include "gl_utils.h"

class Framebuffer
{
private:
	GLuint m_RendererID;
	GLuint m_RenderedTexture;
	GLuint m_DepthBuffer;
	unsigned int m_Width;
	unsigned int m_Height;
	float m_Scale; // allows upscaling the texture/viewport

public:
	Framebuffer(unsigned int width, unsigned int height, float scale = 1.0f);
	~Framebuffer();

	void Bind() const;
	void Unbind() const;

	void UpdateFramebufferSize(unsigned int width, unsigned int height);
	void UpdateFramebufferScale(float scale);
	void UpdateFramebufferSizeAndScale(unsigned int width, unsigned int height, float scale);

	inline GLuint GetTexture() { return m_RenderedTexture; };

private:
	void UpdateFramebuffer();
};