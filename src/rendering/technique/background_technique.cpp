#include "background_technique.hpp"

namespace cgvkp {
namespace rendering {
    bool background_technique::init()
    {
        if (!Technique::init())
        {
            return false;
        }
        if (!addShader(GL_VERTEX_SHADER, "shaders/background.vs"))
        {
            return false;
        }
        if (!addShader(GL_FRAGMENT_SHADER, "shaders/background.fs"))
        {
            return false;
        }
        if (!link())
        {
            return false;
        }

		screenSizeLocation = getUniformLocation("screenSize");

        texture = util::texture::from_png("starrynight.png");
        if (!texture->uploaded()) return false;

		use();
		GLint texSizeLocation = getUniformLocation("textureSize");
		glUniform2i(texSizeLocation, static_cast<GLint>(texture->width()), static_cast<GLint>(texture->height()));

        return true;
    }
    void background_technique::deinit()
    {
        Technique::deinit();
        texture.reset();
    }

	void background_technique::setScreenSize(GLsizei width, GLsizei height) const
	{
		glUniform2i(screenSizeLocation, width, height);
	}

    void background_technique::render() const
    {
        texture->bind_texture();
    }
}
}
