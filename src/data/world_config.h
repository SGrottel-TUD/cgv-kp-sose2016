#pragma once

namespace cgvkp {
namespace data {

	class world_config {
	public:
        world_config();
		~world_config();

		inline float width(void) const {
			return w;
		}
		inline float height(void) const {
			return h;
		}

		inline void set_size(float w, float h) {
			this->w = w;
			this->h = h;
		}

	private:
		/** size of the game area in meter */
		float w, h;
	};

}
}
