#pragma once
#include <string>
#include <memory>
#include <GL/glew.h>

namespace cgvkp {
    namespace util {
        class texture {
        public:
            // Dummy constructor, instances should be created with from_png / ...
            texture();
            virtual ~texture();

            inline size_t width() { return _width; }
            inline size_t height() { return _height; }
            inline GLuint id() { return _id; }
            inline bool uploaded() { return _uploaded; }

            void bind_texture();
            // Static members
            static std::shared_ptr<texture> from_png(std::string const& filename,
                GLint wrap_s = GL_REPEAT,
                GLint wrap_t = GL_REPEAT,
                GLint mag_filter = GL_LINEAR,
                GLint min_filter = 0, // default: GL_LINEAR_MIPMAP_LINEAR (if genMipMaps == false: GL_LINEAR)
                bool genMipMaps = true
            );
        private:
            // Instance members
            size_t _width, _height;
            GLuint _id;
            bool _has_alpha, _uploaded;
            // Static members
            static GLuint upload_and_get_id(
                GLubyte **data,
                GLsizei width,
                GLsizei height,
                bool has_alpha = false,
                GLint wrap_s = GL_REPEAT,
                GLint wrap_t = GL_REPEAT,
                GLint mag_filter = GL_LINEAR,
                GLint min_filter = 0, // default: GL_LINEAR_MIPMAP_LINEAR (if genMipMaps == false: GL_LINEAR)
                bool genMipMaps = true
            );
        };
    }
}