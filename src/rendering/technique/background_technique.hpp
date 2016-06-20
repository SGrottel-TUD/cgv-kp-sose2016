#pragma once

#include "technique.hpp"
#include "util/texture.hpp"

namespace cgvkp
{
    namespace rendering
    {
        class background_technique : public Technique
        {
        public:
            background_technique();
            ~background_technique();
            virtual bool init();
            void deinit();
            void render() const;
        private:
            GLuint vao, vertexBuffers[2];
            std::shared_ptr<util::texture> texture;
        };
    }
}