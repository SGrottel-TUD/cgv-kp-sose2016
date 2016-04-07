#pragma once

namespace cgvkp {
namespace data {

	class world_meta_data {
	public:
		world_meta_data();
		~world_meta_data();

		float get_width(void) const {
			return width;
		}
		float get_height(void) const {
			return height;
		}
		unsigned int get_score(void) const {
			return score;
		}

		void set_size(float w, float h) {
			width = w;
			height = h;
		}
		void set_score(unsigned int s) {
			score = s;
		}
		void inc_score(void) {
			score++;
		}

	private:
		/** size of the game area in meter */
		float width, height;
		/** number for stars collected */
		unsigned int score;
	};

}
}
