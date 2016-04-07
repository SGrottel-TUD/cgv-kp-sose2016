#include "data/meta_data.h"

using namespace cgvkp;

data::meta_data::meta_data()
		: width(1.0f), height(1.0f), score(0) {
}

data::meta_data::~meta_data() {
}

void data::meta_data::copy_from(const meta_data& src) {
    width = src.width;
    height = src.height;
    score = src.score;
}
