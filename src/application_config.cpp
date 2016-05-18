#include "application_config.hpp"
#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream>

cgvkp::application_config::application_config()
	: active_vision(vision_inputs::debug),
	active_renderer(renderers::debug),
	window_width(1280),
	window_height(720),
	camera_mode(rendering::camera_mode::mono),
	eye_separation(0),
	zzero_parallax(0),
	fullscreen(false),
	data_path("config.ini") {
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
		if (strncmp(argv[i], "-rM", 3) == 0) {
			active_renderer = renderers::models;
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
	lhs << "camera_mode = " << rhs.camera_mode << std::endl;
	lhs << "eye_separation = " << rhs.eye_separation << std::endl;
	lhs << "zzero_parallax = " << rhs.camera_mode << std::endl;
	
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
		else if (key.find("camera_mode") != std::string::npos)
		{
			int mode = 0;
			ss >> mode;
			rhs.camera_mode = static_cast<rendering::camera_mode>(mode);
		}
		else if (key.find("eye_separation") != std::string::npos)
		{
			ss >> rhs.eye_separation;
		}
		else if (key.find("zzero_parallax") != std::string::npos)
		{
			ss >> rhs.zzero_parallax;
		}
	}

	return lhs;
}