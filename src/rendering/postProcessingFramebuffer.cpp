#if defined(_DEBUG) || defined(DEBUG)
#include <iostream>
#endif
#include "postProcessingFramebuffer.hpp"

bool cgvkp::rendering::PostProcessingFramebuffer::init(GLsizei numNeededTextures)
{
	numTextures = numNeededTextures;
	pTextures = new GLuint[numTextures];
	glGenTextures(numTextures, pTextures);
	resize(1, 1);	// The textures must contain at least 1 pixel.

	// Create the framebuffer.
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	for (int i = 0; i < numTextures; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, pTextures[i]);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, pTextures[i], 0);

		// There is a 1:1 mapping between a screen pixel and G Buffer texel. Setting the filtering type to GL_NEAREST prevents unnecessary interpolation.
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

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

void cgvkp::rendering::PostProcessingFramebuffer::deinit()
{
	if (numTextures)
	{
		glDeleteTextures(numTextures, pTextures);
		delete[] pTextures;
		pTextures = nullptr;
		numTextures = 0;
	}

	if (framebuffer)
	{
		glDeleteFramebuffers(1, &framebuffer);
	}
}

void cgvkp::rendering::PostProcessingFramebuffer::resize(GLsizei width, GLsizei height) const
{
	glDrawBuffer(GL_BACK);
	for (int i = 0; i < numTextures; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
	}
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);

	for (int i = 0; i < numTextures; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, pTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	}
}

void cgvkp::rendering::PostProcessingFramebuffer::nextPass(int nextDrawBuffer, int numTexturesToBind /* = 1 */) const
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + (nextDrawBuffer % numTextures));

	for (int i = 0; i < numTexturesToBind; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, pTextures[(numTextures + nextDrawBuffer - i - 1) % numTextures]);
	}
}

void cgvkp::rendering::PostProcessingFramebuffer::finalPass(int currentDrawBuffer, int numTexturesToBind /* = 1 */) const
{
	for (int i = 0; i < numTexturesToBind; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, pTextures[(numTextures + currentDrawBuffer - i) % numTextures]);
	}
}
