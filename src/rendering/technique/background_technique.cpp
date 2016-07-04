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

        texture = util::texture::from_png("starrynight.png");
        if (!texture->uploaded()) return false;
        return true;
    }
    void background_technique::deinit()
    {
        Technique::deinit();
        texture.reset();
    }
    void background_technique::render() const
    {
        use();
        texture->bind_texture();
    }
}
}
