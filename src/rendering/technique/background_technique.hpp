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

		private:
			std::shared_ptr<util::texture> texture;
		};
	}
}