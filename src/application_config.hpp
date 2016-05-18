#pragma once
#include "rendering/release_renderer.hpp"
#include <iostream>
#include <iosfwd>

namespace cgvkp {
    class application_config {
    public:
        // Constructors
        application_config();
        application_config(int argc, char **argv);
		application_config(std::string const& path);
		application_config(int argc, char **argv, std::string const& path);

		void load_file(std::string const& path);
		void save_file(std::string const& path);


        // enum classes
        enum class renderers : unsigned int {
            debug = 0u,
            release = 1u,
            models = 2u
        };
        enum class vision_inputs : unsigned int {
            debug = 0u,
            release = 1u,
            dummy = 2u
        };

        // Properties
        renderers active_renderer;
        vision_inputs active_vision;
		std::string data_path;

		int window_width;
		int window_height;
		bool fullscreen;
		rendering::camera_mode camera_mode;
		float eye_separation;
		float zzero_parallax;

	private:
		void interpret_arguments(int argc, char** argv);
    };

	std::ostream& operator<<(std::ostream& lhs, application_config const& rhs);

	std::istream& operator>>(std::istream& lhs, application_config& rhs);

}
