#include "data/world.hpp"

//#include "GL/glew.h"
//#include "GLFW/glfw3.h"
//#include "glm/glm.hpp"
//#include "glm/gtx/transform.hpp"
//#include "glm/gtc/type_ptr.hpp"
//#include <iostream>
//#include <cassert>
//#include <chrono>
//
//namespace {
//
//    void APIENTRY my_ogl_debug_func(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) {
//        const char * type_str = "Default";
//        const char *sever_str = "Default";
//        switch (type) {
//        case GL_DEBUG_TYPE_ERROR_ARB: type_str = "Error"; break;
//        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: type_str = "Deprecated Behavior"; break;
//        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: type_str = "Undefined Behavior"; break;
//            //case GL_DEBUG_TYPE_PORTABILITY_ARB: type_str = "Portability"; break;
//            //case GL_DEBUG_TYPE_PERFORMANCE_ARB: type_str = "Performance"; break;
//            //case GL_DEBUG_TYPE_OTHER_ARB: type_str = "Other"; break;
//        default: return;
//        }
//        switch (severity) {
//        case GL_DEBUG_SEVERITY_HIGH_ARB: sever_str = "High"; break;
//        case GL_DEBUG_SEVERITY_MEDIUM_ARB: sever_str = "Medium"; break;
//        case GL_DEBUG_SEVERITY_LOW_ARB: sever_str = "Low"; break;
//            //default: return;
//        }
//
//        std::cerr << "ogl_dbg: " << type_str << ", " << sever_str << ", " << message << std::endl;
//    }
//
//}
//
//void printShaderInfoLog(GLuint obj) {
//    int infologLength = 0;
//    int charsWritten = 0;
//    char *infoLog;
//    ::glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
//    if (infologLength > 0) {
//        infoLog = (char *)malloc(infologLength);
//        ::glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
//        std::cout << infoLog << std::endl;
//        free(infoLog);
//    }
//}
//
//void printProgramInfoLog(GLuint obj) {
//    int infologLength = 0;
//    int charsWritten = 0;
//    char *infoLog;
//    ::glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
//    if (infologLength > 0) {
//        infoLog = (char *)malloc(infologLength);
//        ::glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
//        std::cout << infoLog << std::endl;
//        free(infoLog);
//    }
//}

int main(int argc, char **argv) {
    cgvkp::data::world data;


/*
	std::cout << "Started" << std::endl;

    //
    // init glfw window
    if (::glfwInit() != GL_TRUE) {
        std::cerr << "glfwInit failed" << std::endl;
        return -1;
    }

    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // ogl 3.3 core
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    ::glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    ::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = ::glfwCreateWindow(1280, 720, "OGL 3.3 Core Test", NULL, NULL);
    if (!window) {
        std::cerr << "glfwCreateWindow failed" << std::endl;
        ::glfwTerminate();
        return -1;
    }

    ::glfwMakeContextCurrent(window);

    //
    // init glew
    ::glewExperimental = GL_TRUE;
    GLenum err = ::glewInit();
    if (err != GLEW_OK) {
        std::cerr << "glewInit failed" << std::endl;
        ::glfwTerminate();
        return -1;
    }
    if (!::glewIsSupported("GL_VERSION_3_3")) {
        std::cerr << "glewIsSupported(GL_VERSION_3_3) failed" << std::endl;
        ::glfwTerminate();
        return -1;
    }

#ifdef GL_KHR_debug
    if (glewIsSupported("GL_KHR_debug")) {
        glDebugMessageCallback((GLDEBUGPROC)&my_ogl_debug_func, nullptr);
        glEnable(GL_DEBUG_OUTPUT);
    }
#elif GL_ARB_debug_output
    if (glewIsSupported("GL_ARB_debug_output")) {
        glDebugMessageCallbackARB((GLDEBUGPROCARB)&my_ogl_debug_func, nullptr);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    }
#endif

    //
    // init graphics data
    ::glClearColor(0.95f, 0.95f, 0.95f, 0.0f); // dark blue background

                                               // Texture
    GLuint tex;
    ::glGenTextures(1, &tex);
    ::glBindTexture(GL_TEXTURE_2D, tex);

    const int img_width = 4;
    const int img_height = 4;
    unsigned char img[img_width * img_height * 3];
    for (int x = 0; x < img_width; ++x) {
        for (int y = 0; y < img_height; ++y) {
            unsigned char *p = img + (x + y * img_width) * 3;
            if ((x % 2) == (y % 2)) {
                p[0] = 0;
                p[1] = 0;
                p[2] = 0;
            } else {
                p[0] = 255;
                p[1] = 255;
                p[2] = 255;
            }
        }
    }

    ::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    ::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_BGR, GL_UNSIGNED_BYTE, img);

    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    ::glBindTexture(GL_TEXTURE_2D, 0);

    // Shaders
    const char *vert_src = "#version 330 core\n\
\n\
uniform mat4 viewMatrix, projMatrix;\n\
\n\
in vec4 position;\n\
in vec3 color;\n\
\n\
out vec3 frag_color;\n\
out vec2 frag_tex_coord;\n\
\n\
void main() {\n\
    frag_color = color;\n\
    frag_tex_coord = color.xy;\n\
    gl_Position = projMatrix * viewMatrix * position;\n\
}";
    const char *frag_src = "#version 330 core\n\
\n\
uniform sampler2D tex;\n\
\n\
in vec3 frag_color;\n\
in vec2 frag_tex_coord;\n\
out vec4 output_color;\n\
\n\
void main() {\n\
    output_color = vec4(frag_color * texture(tex, frag_tex_coord).rgb, 1.0);\n\
}";
    GLuint p, v, f;
    v = ::glCreateShader(GL_VERTEX_SHADER);
    f = ::glCreateShader(GL_FRAGMENT_SHADER);
    ::glShaderSource(v, 1, &vert_src, nullptr);
    ::glCompileShader(v);
    printShaderInfoLog(v);
    ::glShaderSource(f, 1, &frag_src, nullptr);
    ::glCompileShader(f);
    printShaderInfoLog(f);

    p = ::glCreateProgram();
    ::glAttachShader(p, v);
    ::glAttachShader(p, f);

    ::glBindFragDataLocation(p, 0, "output_color");
    ::glLinkProgram(p);
    printProgramInfoLog(p);

    // Vertex Attribute Locations
    GLuint vertexLoc, colorLoc;
    vertexLoc = ::glGetAttribLocation(p, "position");
    colorLoc = ::glGetAttribLocation(p, "color");

    // Uniform variable Locations
    GLuint projMatrixLoc, viewMatrixLoc, texLoc;
    projMatrixLoc = ::glGetUniformLocation(p, "projMatrix");
    viewMatrixLoc = ::glGetUniformLocation(p, "viewMatrix");
    texLoc = ::glGetUniformLocation(p, "tex");

    const float vertices1[] = { 1.0f, -1.0f, 0.0f, 1.0f,
        0.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 1.0f };
    const float colors1[] = { 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f };
    GLuint vao;
    GLuint buffers[2];

    ::glGenVertexArrays(1, &vao);
    ::glBindVertexArray(vao);
    // Generate two slots for the vertex and color buffers
    ::glGenBuffers(2, buffers);
    // bind buffer for vertices and copy data into buffer
    ::glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    ::glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    ::glEnableVertexAttribArray(vertexLoc);
    ::glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, 0, 0, 0);

    // bind buffer for colors and copy data into buffer
    ::glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    ::glBufferData(GL_ARRAY_BUFFER, sizeof(colors1), colors1, GL_STATIC_DRAW);
    ::glEnableVertexAttribArray(colorLoc);
    ::glVertexAttribPointer(colorLoc, 4, GL_FLOAT, 0, 0, 0);

    glm::mat4 proj(1.0f), view(1.0f);

    // ::glEnable(GL_CULL_FACE);

    std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
    int win_width(-1), win_height(-1);
    //
    // now the main loop
    while (!::glfwWindowShouldClose(window)) {
        { // Update here
            double seconds = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start_time).count();
            view = ::glm::rotate(
                ::glm::lookAt(::glm::vec3(0.0f, 0.0f, 6.0f), ::glm::vec3(0.0f), ::glm::vec3(0.0f, 1.0f, 0.0f)),
                ::glm::radians(90.0f * static_cast<float>(seconds)), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        { // ensure viewport
            int frame_width, frame_height;
            ::glfwGetFramebufferSize(window, &frame_width, &frame_height);
            if (frame_height <= 0) frame_height = 1;
            if ((win_width != frame_width) || (win_height != frame_height)) {
                win_width = frame_width;
                win_height = frame_height;
                ::glViewport(0, 0, win_width, win_height);
                proj = ::glm::perspective(::glm::radians(30.0f),
                    static_cast<float>(win_width) / static_cast<float>(win_height), 0.01f, 100.0f);
            }
        }

        // Render here
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ::glUseProgram(p);

        ::glUniformMatrix4fv(projMatrixLoc, 1, false, glm::value_ptr(proj));
        ::glUniformMatrix4fv(viewMatrixLoc, 1, false, glm::value_ptr(view));

        ::glActiveTexture(GL_TEXTURE0);
        ::glBindTexture(GL_TEXTURE_2D, tex);
        ::glUniform1i(texLoc, 0);

        ::glBindVertexArray(vao);
        ::glDrawArrays(GL_TRIANGLES, 0, 3);

        // done rendering. swap and next turn
        ::glfwSwapBuffers(window);
        ::glfwPollEvents();
    }

    //
    // cleanup
    ::glUseProgram(0);
    ::glBindVertexArray(0);

    ::glDeleteProgram(p);
    ::glDeleteShader(v);
    ::glDeleteShader(f);

    ::glDeleteBuffers(2, buffers);
    ::glDeleteVertexArrays(1, &vao);
    ::glDeleteTextures(1, &tex);

    ::glfwTerminate();
*/
    return 0;
}
