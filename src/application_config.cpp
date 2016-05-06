#include "application_config.hpp"
#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream>

cgvkp::application_config::application_config()
    : active_vision(vision_inputs::debug), active_renderer(renderers::debug), window_width(1280), window_height(720) {
    // Intentionally empty
}

cgvkp::application_config::application_config(int argc, char **argv)
    : application_config::application_config()
{
	interpret_arguments(argc, argv);
}

cgvkp::application_config::application_config(std::string const& path)
	: application_config()
{
	load_file(path);
}

cgvkp::application_config::application_config(int argc, char **argv, std::string const& path)
	: application_config()
{
	load_file(path);
	interpret_arguments(argc, argv);
}

void cgvkp::application_config::interpret_arguments(int argc, char** argv)
{
	for (int i = 0; i < argc; ++i) {
		if (strncmp(argv[i], "-rR", 3) == 0) {
			active_renderer = renderers::release;
		}
		if (strncmp(argv[i], "-vR", 3) == 0) {
			active_vision = vision_inputs::release;
		}
		if (strncmp(argv[i], "-vD", 3) == 0) {
			active_vision = vision_inputs::dummy;
		}
	}
}

void cgvkp::application_config::load_file(std::string const& path)
{
	std::ifstream file(path, std::ios::in);

	if (file.is_open())
	{
		file >> *this;
	}
}

void cgvkp::application_config::save_file(std::string const& path)
{
	std::ofstream file(path, std::ios::trunc);
	file << *this;
}

std::ostream& cgvkp::operator<<(std::ostream& lhs, application_config const& rhs)
{
	lhs << "fullscreen = " << (rhs.fullscreen ? 1 : 0) << std::endl;
	lhs << "width = " << rhs.window_width << std::endl;
	lhs << "heigth = " << rhs.window_height << std::endl;
	lhs << "data_path = \"" << rhs.data_path << "\"" << std::endl;
	
	return lhs;
}

std::istream& cgvkp::operator>>(std::istream& lhs, application_config& rhs)
{
	std::string key;
	std::string value;

	while (std::getline(lhs, key, '='))
	{
		std::getline(lhs, value);
		std::stringstream ss;
		ss << value;

		if (key.find("fullscreen") != std::string::npos)
		{
			ss >> rhs.fullscreen;
		}
		else if (key.find("width") != std::string::npos)
		{
			ss >> rhs.window_width;
		}
		else if (key.find("height") != std::string::npos)
		{
			ss >> rhs.window_height;
		}
		else if (key.find("data_path") != std::string::npos)
		{
			std::string idc;
			std::getline(ss, idc, '"');
			std::getline(ss, rhs.data_path, '"');
		}
	}

	return lhs;
}