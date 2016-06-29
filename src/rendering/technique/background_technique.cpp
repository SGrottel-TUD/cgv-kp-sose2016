#include "background_technique.hpp"

namespace cgvkp {
namespace rendering {
    background_technique::background_technique() : Technique(), texture() {}
    background_technique::~background_technique() { deinit(); }

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

        texture = util::texture::from_png("sky_night5.png");
        if (!texture->uploaded()) return false;

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        float vertices[] = {
            -1.0f, -1.0f,
            1.0f, -1.0f,
            1.0f, 1.0f,
            -1.0f, 1.0f,
        };

        glGenBuffers(2, vertexBuffers);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffers[0]);
        GLushort ind[] = { 1, 2, 0, 3 };
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 4, ind, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[1]);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBindVertexArray(0);
        return true;
    }
    void background_technique::deinit()
    {
        Technique::deinit();
        texture.reset();
		glDeleteBuffers(2, vertexBuffers);
    }
    void background_technique::render() const
    {
        use();
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
		glBindVertexArray(vao);
        texture->bind_texture();
		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, nullptr);
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    }
}
}
