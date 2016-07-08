#include "background_technique.hpp"

namespace cgvkp {
namespace rendering {
    bool background_technique::init()
    {
        if (!Technique::init("background.vs", "background.fs"))
        {
            return false;
        }

		scaleLocation = getUniformLocation("scale");

        texture = util::texture::from_png("starrynight.png");
        if (!texture->uploaded()) return false;

        return true;
    }
    void background_technique::deinit()
    {
        Technique::deinit();
        texture.reset();
    }

	void background_technique::setScreenSize(GLsizei width, GLsizei height) const
	{
		glUniform2f(scaleLocation, static_cast<float>(width) / texture->width(), static_cast<float>(height) / texture->height());
	}

    void background_technique::render() const
    {
        texture->bind_texture();
    }
}
}
