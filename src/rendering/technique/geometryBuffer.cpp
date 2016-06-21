#if defined(_DEBUG) || defined(DEBUG)
#include <iostream>
#endif
#include "geometrybuffer.hpp"
#include "rendering/lights.hpp"
#include "rendering/model/graphic_model_base.hpp"
#include <memory>

/*
	GBuffer
*/

cgvkp::rendering::GeometryBuffer::GeometryBuffer()
	: framebuffer(0), depthStencilTexture(0)
{
	for (int i = 0; i < numTextures; ++i)
	{
		textures[i] = 0;
	}
}

cgvkp::rendering::GeometryBuffer::~GeometryBuffer()
{
	deinit();
}

bool cgvkp::rendering::GeometryBuffer::init(GLsizei width, GLsizei height)
{
	// Create the GBuffer textures.
	glGenTextures(numTextures, textures);
	glGenTextures(1, &depthStencilTexture);
	resize(width, height);

	// Create the framebuffer.
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	for (int i = 0; i < final; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);

		// There is a 1:1 mapping between a screen pixel and G Buffer texel. Setting the filtering type to GL_NEAREST prevents unnecessary interpolation.
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	// Final image.
	glBindTexture(GL_TEXTURE_2D, textures[final]);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + final, GL_TEXTURE_2D, textures[final], 0);

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

void cgvkp::rendering::GeometryBuffer::bindForGeometryPass() const
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
	glDrawBuffers(numTextures, DrawBuffers);
}

void cgvkp::rendering::GeometryBuffer::bindForLightPass() const
{
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + final);

	for (int i = 0; i < final; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
	}
}

void cgvkp::rendering::GeometryBuffer::bindForFinalPass() const
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + final);
}

void cgvkp::rendering::GeometryBuffer::resize(GLsizei width, GLsizei height) const
{
	glDrawBuffer(GL_BACK);
	glReadBuffer(GL_NONE);
	for (int i = 0; i < GeometryBuffer::final; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
	}
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, GL_NONE);

	for (int i = 0; i < final; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	}

	// Final image.
	glBindTexture(GL_TEXTURE_2D, textures[final]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_FLOAT, nullptr);

	// Depth/Stencil buffer.
	glBindTexture(GL_TEXTURE_2D, depthStencilTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, nullptr);
}



/*
	Geometry pass technique
*/

cgvkp::rendering::GeometryTechnique::GeometryTechnique()
	: ambientLightLocation(invalidLocation), materialLocation(invalidLocation), worldLocation(invalidLocation)
{
}

bool cgvkp::rendering::GeometryTechnique::init()
{
	if (!Technique::init())
	{
		return false;
	}
	if (!addShader(GL_VERTEX_SHADER, "shaders/geometry.vs"))
	{
		return false;
	}
	if (!addShader(GL_FRAGMENT_SHADER, "shaders/geometry.fs"))
	{
		return false;
	}
	if (!link())
	{
		return false;
	}

	ambientLightLocation = getUniformLocation("ambientLight");
	materialLocation = getUniformLocation("material");
	worldViewProjectionLocation = getUniformLocation("worldViewProjection");
	worldLocation = getUniformLocation("world");

	if (ambientLightLocation == invalidLocation ||
		materialLocation == invalidLocation ||
		worldViewProjectionLocation == invalidLocation ||
		worldLocation == invalidLocation)
	{
		return false;
	}

	return true;
}

void cgvkp::rendering::GeometryTechnique::setAmbientLight(glm::vec3 const& color) const
{
	glUniform3f(ambientLightLocation, color.r, color.g, color.b);
}

void cgvkp::rendering::GeometryTechnique::setMaterial() const
{
	float specularPower = 1;
	float specularIntensity = 0;
	glUniform2f(materialLocation, specularPower, specularIntensity);
}

void cgvkp::rendering::GeometryTechnique::setWorld(glm::mat4x4 const& world) const
{
	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, &world[0][0]);
}
bool cgvkp::rendering::GeometryTechnique::renderView(view::view_base::ptr const view, glm::mat4x4 const& projectionViewMatrix) const
{
    if (!view->is_valid() || !view->has_model()) return false;
    auto graphic_model = std::dynamic_pointer_cast<model::graphic_model_base>(view->get_model());
    if (graphic_model == nullptr) return false;
    setWorld(graphic_model->model_matrix);
    setWorldViewProjection(projectionViewMatrix * graphic_model->model_matrix);
    view->render();
    return true;
}
