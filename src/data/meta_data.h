#pragma once

namespace cgvkp {
namespace data {

	class meta_data {
	public:
		meta_data();
		~meta_data();

        void copy_from(const meta_data& src);

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
