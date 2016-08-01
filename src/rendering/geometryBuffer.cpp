#if defined(_DEBUG) || defined(DEBUG)
#include <iostream>
#endif
#include "geometrybuffer.hpp"

cgvkp::rendering::GeometryBuffer::GeometryBuffer()
	: framebuffer(0), depthStencilTexture(0)
{
	for (int i = 0; i < numTextures; ++i)
	{
		textures[i] = 0;
	}
}

bool cgvkp::rendering::GeometryBuffer::init()
{
	// Create the GBuffer textures.
	glGenTextures(numTextures, textures);
	glGenTextures(1, &depthStencilTexture);
	resize(1, 1);	// The textures must contain at least 1 pixel.

	// Create the framebuffer.
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	for (int i = 0; i < numTextures; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);

		// There is a 1:1 mapping between a screen pixel and G Buffer texel. Setting the filtering type to GL_NEAREST prevents unnecessary interpolation.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	// Depth/Stencil buffer.
	glBindTexture(GL_TEXTURE_2D, depthStencilTexture);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilTexture, 0);

	// Check framebuffer.
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
#if defined(_DEBUG) || defined(DEBUG)
		std::cerr << "Framebuffer not complete (status: " << status << ")." << std::endl;
#endif
		return false;
	}

	// Restore default framebuffer.
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	return true;
}

void cgvkp::rendering::GeometryBuffer::deinit()
{
	if (textures[0])
	{
		glDeleteTextures(numTextures, textures);
		for (int i = 0; i < numTextures; ++i)
		{
			textures[i] = 0;
		}
	}

	if (depthStencilTexture)
	{
		glDeleteTextures(1, &depthStencilTexture);
		depthStencilTexture = 0;
	}

	if (framebuffer)
	{
		glDeleteFramebuffers(1, &framebuffer);
	}
}

void cgvkp::rendering::GeometryBuffer::bindForWritingGeometry() const
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
	glDrawBuffers(final, DrawBuffers);
}

void cgvkp::rendering::GeometryBuffer::bindForReadingGeometry() const
{
	for (int i = 0; i < numTextures; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
	}
}

void cgvkp::rendering::GeometryBuffer::bindForWritingFinal() const
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + final);
}

void cgvkp::rendering::GeometryBuffer::bindForReadingFinal() const
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + final);
}

void cgvkp::rendering::GeometryBuffer::resize(GLsizei width, GLsizei height) const
{
	glDrawBuffer(GL_BACK);
	glReadBuffer(GL_NONE);
	for (int i = 0; i < numTextures; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
	}
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, GL_NONE);

	for (int i = 0; i < numTextures; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	}

	// Depth/Stencil buffer.
	glBindTexture(GL_TEXTURE_2D, depthStencilTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, nullptr);
}
