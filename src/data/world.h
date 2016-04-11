#pragma once
#include "data/world_config.h"
#include "data/input_layer.h"
#include "data/tripple_buffer.h"
//#include <vector>
//#include "data/hand.h"
//#include "data/star.h"

namespace cgvkp {
namespace data {

	class world {
	public:
		world();
		~world();

        void init();

    private:
        world_config cfg;
        tripple_buffer<input_layer::hand_collection> hands_input;
        input_layer input;

        //std::vector<hand::ptr_type> hands;
        //std::vector<star::ptr_type> stars;
	};

}
}
