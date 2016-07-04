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
			bool init();
			void deinit();
			void render() const;
			void setScreenSize(GLsizei width, GLsizei height) const;

		private:
			GLint screenSizeLocation;
			std::shared_ptr<util::texture> texture;
		};
	}
}
