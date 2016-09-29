#pragma once
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
        enum class vision_inputs : unsigned int {
            debug = 0,
            release,
            dummy
        };
        enum CameraMode
        {
            mono = 0,
            stereo = 1
        };

        // Properties
        vision_inputs active_vision;

		bool debug;
		std::string resourcesBasePath;
		int windowWidth;
		int windowHeight;
		int windowPosx;
		int windowPosy;
		bool fullscreen;
		int fullscreenWidth;
		int fullscreenHeight;
		CameraMode cameraMode;
		float eyeSeparation;
		float zZeroParallax;
		bool vSync;

	private:
		void interpret_arguments(int argc, char** argv);
    };

	std::ostream& operator<<(std::ostream& lhs, application_config::vision_inputs const& rhs);
	std::ostream& operator<<(std::ostream& lhs, application_config const& rhs);
	std::istream& operator>>(std::istream& lhs, application_config& rhs);
}
