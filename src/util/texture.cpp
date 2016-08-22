#include "texture.hpp"
#include "util/resource_file.hpp"
#include <stdlib.h>
#include <png.h>
#include <iostream>

// Partially based on:
// https://blog.nobel-joergensen.com/2010/11/07/loading-a-png-as-texture-in-opengl-using-libpng/

namespace cgvkp
{
namespace util
{
    texture::texture() : _width(0u), _height(0u), _id(0u), _has_alpha(false), _uploaded(false) {}
    texture::~texture()
    {
        if (_uploaded)
        {
            ::glDeleteTextures(1, &_id);
        }
    }
    void texture::bind_texture() {
        ::glActiveTexture(GL_TEXTURE0);
        ::glBindTexture(GL_TEXTURE_2D, id());
    }
    GLuint texture::upload_and_get_id(
        GLubyte **data,
        GLsizei width,
        GLsizei height,
        bool has_alpha,
        GLint wrap_s,
        GLint wrap_t,
        GLint mag_filter,
        GLint min_filter, // default: GL_LINEAR_MIPMAP_LINEAR (if genMipMaps == false: GL_LINEAR)
        bool genMipMaps)
    {
        GLuint id;
        ::glGenTextures(1, &id);

        glBindTexture(GL_TEXTURE_2D, id);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, (has_alpha) ? GL_RGBA : GL_RGB, width, height, 0, (has_alpha) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, *data);

        if (min_filter == 0) min_filter = genMipMaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);

        if (genMipMaps) glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        return id;
    }
    // Tons of C-Like code needed by libpng
    std::shared_ptr<texture> texture::from_png(std::string const& filename,
        GLint wrap_s, GLint wrap_t, GLint mag_filter, GLint min_filter, bool genMipMaps)
    {
        // Uninitialised texture object to return if something fails
        std::shared_ptr<texture> tex = std::make_shared<texture>();
        // Read stuff
        png_structp png_ptr;
        png_infop info_ptr;
        unsigned int sig_read = 0;
        int color_type, interlace_type;
        FILE *fp;
        if (fopen_s(&fp, filename.data(), "rb") != 0)
        {
            // Open failed, try again using resource_file
            if (fopen_s(&fp, resource_file::getResourcePath("textures", filename).data(), "rb") != 0)
            {
                std::cerr << "Could not open texture: '" << filename << "'" << std::endl;
                return tex;
            }
        }
        // TODO: Maybe cleaner function add callbacks for error / warning handling in libpng
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (png_ptr == NULL)
        {
            fclose(fp);
            std::cerr << "Wrong libpng library version!" << std::endl;
            return tex;
        }
        // Allocate memory
        info_ptr = png_create_info_struct(png_ptr);
        if (info_ptr == NULL)
        {
            fclose(fp);
            png_destroy_read_struct(&png_ptr, NULL, NULL);
            std::cerr << "Could not allocate memory for libpng struct" << std::endl;
            return tex;
        }
        // Error handling without callbacks
        if (setjmp(png_jmpbuf(png_ptr)))
        {
            // This means the file could not be read
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            fclose(fp);
            std::cerr << "Could not read texture: '" << filename << "'" << std::endl;
            return tex;
        }

        png_init_io(png_ptr, fp);
        png_set_sig_bytes(png_ptr, sig_read);
        // Do read
        png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

        png_uint_32 w, h;
        int bit_depth;
        png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, &interlace_type, NULL, NULL);

        size_t row_bytes = png_get_rowbytes(png_ptr, info_ptr);

        GLubyte* data = (unsigned char*)malloc(row_bytes * h);
        png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
        for (size_t i = 0u; i < h; ++i)
        {
            // png is top --> bottom, invert cords
            size_t j = h - (i + 1u);
            memcpy(data + row_bytes * j, row_pointers[i], row_bytes);
        }
        // Cleanup
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);

        // Set texture values
        tex->_width = w;
        tex->_height = h;
        tex->_has_alpha = ((color_type & PNG_COLOR_MASK_ALPHA) != 0);
        // Finally upload texture
        tex->_id = texture::upload_and_get_id(&data, (GLsizei)tex->width(), (GLsizei)tex->height(), tex->_has_alpha);
        tex->_uploaded = true;
        // Release memory
        free(data);
        // Return texture
#if defined(_DEBUG) || defined(DEBUG)
        std::cout << "texture '" << filename << "' uploaded" << std::endl;
#endif
        return tex;
    }
}
}
