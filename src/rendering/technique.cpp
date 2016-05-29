#include "technique.hpp"
#include "util/resource_file.hpp"
#include <string>
#if defined(_DEBUG) || defined(DEBUG)
#include <iostream>
#endif

cgvkp::rendering::Technique::Technique()
	: program(0)
{
}

cgvkp::rendering::Technique::~Technique()
{
	deinit();
}

bool cgvkp::rendering::Technique::init()
{
	program = glCreateProgram();
	if (!program)
	{
#if defined(_DEBUG) || defined(DEBUG)
		std::cerr << "Could not create a program." << std::endl;
#endif
		return false;
	}

	return true;
}

void cgvkp::rendering::Technique::deinit()
{
	if (program)
	{
		glDeleteProgram(program);
		program = 0;
	}

	// If there are shaders left they were not linked to a program. Delete them.
	for (GLuint shader : shaders)
	{
		glDeleteShader(shader);
	}
	shaders.clear();
}

bool cgvkp::rendering::Technique::addShader(GLenum shaderType, char const* filename)
{
	// Get shader source.
	std::string source;
	if (!util::resource_file::read_file_as_text(filename, source))
	{
#if defined(_DEBUG) || defined(DEBUG)
		std::cerr << "Could not read file \"" << filename << "\"." << std::endl;
#endif
		return false;
	}
	GLchar const* sourceCStr = source.c_str();

	// Create shader.
	GLuint shader = glCreateShader(shaderType);
	if (!shader)
	{
#if defined(_DEBUG) || defined(DEBUG)
		std::cerr << "Could not create shader with type " << shaderType << '.' << std::endl;
#endif
		return false;
	}
	shaders.push_back(shader);

	// Compile shader.
	glShaderSource(shader, 1, &sourceCStr, nullptr);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE)
	{
#if defined(_DEBUG) || defined(DEBUG)
		std::cerr << "Could not compile shader \"" << filename << "\" (type: " << shaderType << ")." << std::endl;
#endif
		return false;
	}

	// Attach shader to the program. It will be linked in FUNCTION.
	glAttachShader(program, shader);

	return true;
}

GLint cgvkp::rendering::Technique::getUniformLocation(GLchar const* name) const
{
	GLint location = glGetUniformLocation(program, name);

#if defined(_DEBUG) || defined(DEBUG)
	if (location == -1)
	{
		std::cerr << "Could not find location for uniform \"" << name << "\"." << std::endl;
	}
#endif

	return location;
}

bool cgvkp::rendering::Technique::link()
{
	glLinkProgram(program);

	// Check if program could be linked.
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
#if defined(_DEBUG) || defined(DEBUG)
		std::cerr << "Could not link progam." << std::endl;
#endif
		return false;
	}

	// Check if program is valid.
	glValidateProgram(program);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &success);

	if (success == GL_FALSE)
	{
#if defined(_DEBUG) || defined(DEBUG)
		std::cerr << "Progam is not valid." << std::endl;
#endif
		return false;
	}

	// Delete the shaders. They are not needed anymore.
	for (GLuint shader : shaders)
	{
		glDeleteShader(shader);
	}
	shaders.clear();

	return true;
}
