#include "application_config.hpp"
#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream>

const cgvkp::application_config defaultConfig;

cgvkp::application_config::application_config()
	: active_vision(vision_inputs::debug),
	debug(true),	// Should be changed for shipping.
	resourcesBasePath("src/resources"),
	windowWidth(1280),
	windowHeight(720),
	cameraMode(rendering::release_renderer::CameraMode::mono),
	eyeSeparation(0),
	zZeroParallax(0),
	fullscreen(false)
{
    // Intentionally empty
}

cgvkp::application_config::application_config(int argc, char **argv)
	: application_config()
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
	std::ifstream file(path);

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
	lhs << (rhs.debug == defaultConfig.debug ? "#" : "") << "debug = " << (rhs.debug ? 1 : 0) << std::endl;
	lhs << (rhs.resourcesBasePath == defaultConfig.resourcesBasePath ? "#" : "") << "resourcesBasePath = \"" << rhs.resourcesBasePath << "\"" << std::endl;
	lhs << (rhs.fullscreen == defaultConfig.fullscreen ? "#" : "") << "fullscreen = " << (rhs.fullscreen ? 1 : 0) << std::endl;
	lhs << (rhs.windowWidth == defaultConfig.windowWidth ? "#" : "") << "windowWidth = " << rhs.windowWidth << std::endl;
	lhs << (rhs.windowHeight == defaultConfig.windowHeight ? "#" : "") << "windowHeigth = " << rhs.windowHeight << std::endl;
	lhs << (rhs.cameraMode == defaultConfig.cameraMode ? "#" : "") << "cameraMode = " << rhs.cameraMode << std::endl;
	lhs << (rhs.eyeSeparation == defaultConfig.eyeSeparation ? "#" : "") << "eyeSeparation = " << rhs.eyeSeparation << std::endl;
	lhs << (rhs.cameraMode == defaultConfig.cameraMode ? "#" : "") << "zZeroParallax = " << rhs.zZeroParallax << std::endl;
	
	return lhs;
}

std::istream& cgvkp::operator>>(std::istream& lhs, application_config& rhs)
{
	std::string key;
	std::string value;

	while (std::getline(lhs, key, '='))
	{
		if (key[0] == '#')	// Comment
		{
			std::getline(lhs, key);	// key will be overwritten in the next iteration.
			continue;
		}

		std::getline(lhs, value);
		std::stringstream ss;
		ss << value;

		if (key.find("debug") != std::string::npos)
		{
			ss >> rhs.debug;
		}
		else if (key.find("resourcesBasePath") != std::string::npos)
		{
			std::string ignore;
			std::getline(ss, ignore, '"');
			std::getline(ss, rhs.resourcesBasePath, '"');
		}
		else if (key.find("fullscreen") != std::string::npos)
		{
			ss >> rhs.fullscreen;
		}
		else if (key.find("windoWidth") != std::string::npos)
		{
			ss >> rhs.windowWidth;
		}
		else if (key.find("windowHeight") != std::string::npos)
		{
			ss >> rhs.windowHeight;
		}
		else if (key.find("cameraMode") != std::string::npos)
		{
			int mode = 0;
			ss >> mode;
			rhs.cameraMode = static_cast<rendering::release_renderer::CameraMode>(mode);
		}
		else if (key.find("eyeSeparation") != std::string::npos)
		{
			ss >> rhs.eyeSeparation;
		}
		else if (key.find("zZeroParallax") != std::string::npos)
		{
			ss >> rhs.zZeroParallax;
		}
	}

	return lhs;
}