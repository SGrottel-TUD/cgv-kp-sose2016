#pragma once
#include "data/world_config.h"
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
        //std::vector<hand::ptr_type> hands;
        //std::vector<star::ptr_type> stars;
	};

}
}
