#include "framebuffer.h"

#include <iostream>

Framebuffer::Framebuffer(unsigned int width, unsigned int height, float scale /*= 1.0f*/)
	: m_RendererID(0), m_RenderedTexture(0), m_DepthBuffer(0), m_Width(width), m_Height(height), m_Scale(scale)
{
	/* Generate the framebuffer */
	GLCall(glGenFramebuffers(1, &m_RendererID));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

	/* Create and bind the texture we will render to */
	GLCall(glGenTextures(1, &m_RenderedTexture));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RenderedTexture));

	/* Give an empty image to the texture (the last 0) */
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GLsizei(m_Scale * m_Width), GLsizei(m_Scale * m_Height), 0, GL_RGB, GL_UNSIGNED_BYTE, 0));

	/* Set filtering for the texture (this is necessary!) */
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	
	/* Other (optional) params... */
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

	/* Setup the depth buffer */
	GLCall(glGenRenderbuffers(1, &m_DepthBuffer));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBuffer));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, GLsizei(m_Scale * m_Width), GLsizei(m_Scale * m_Height)));
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthBuffer));

	/* Set "renderedTexture" as our color attachment #0 */
	GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_RenderedTexture, 0));

	/* Set the list of draw buffers. */
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	GLCall(glDrawBuffers(1, DrawBuffers)); // "1" is the size of DrawBuffers

	/* Check that our framebuffer is ok */
	GLCall(if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) std::cout << "ERROR creating framebuffer!" << std::endl; );

	/* Unbind the frame buffer and texture */
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

Framebuffer::~Framebuffer()
{

}

void Framebuffer::Bind() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
}

void Framebuffer::Unbind() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void Framebuffer::UpdateFramebufferSize(unsigned int width, unsigned int height)
{
	m_Width = width;
	m_Height = height;
	UpdateFramebuffer();
}


void Framebuffer::UpdateFramebufferScale(float scale)
{
	m_Scale = scale;
	UpdateFramebuffer();
}

void Framebuffer::UpdateFramebufferSizeAndScale(unsigned int width, unsigned int height, float scale)
{
	m_Width = width;
	m_Height = height;
	m_Scale = scale;
	UpdateFramebuffer();
}

void Framebuffer::UpdateFramebuffer()
{
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RenderedTexture));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GLsizei(m_Scale * m_Width), GLsizei(m_Scale * m_Height), 0, GL_RGB, GL_UNSIGNED_BYTE, 0));
	GLCall(glViewport(0, 0, GLsizei(m_Scale * m_Width), GLsizei(m_Scale * m_Height)));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBuffer));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, GLsizei(m_Scale * m_Width), GLsizei(m_Scale * m_Height)));
}
