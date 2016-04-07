#pragma once
#include "data/meta_data.h"

namespace cgvkp {
namespace data {

	class world {
	public:
		world();
		~world();

        void init();
        void copy_from(const world& src);

        meta_data& get_meta_data(void) {
            return meta;
        }
        const meta_data& get_meta_data(void) const {
            return meta;
        }

    private:
        meta_data meta;

	};

}
}
