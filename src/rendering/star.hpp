#pragma once
#include "unit.hpp"
#include "light.hpp"
#include "model.hpp"

namespace cgvkp {
	namespace rendering {
		class star : public unit {
		public:
			star(model const& m, light& l);
			void set_position(glm::vec3 const& position);

		private:
			light& l;
		};
	}
}
