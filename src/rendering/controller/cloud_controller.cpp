#include "rendering/controller/cloud_controller.hpp"

#include "rendering/view/cloud_view.hpp"

namespace cgvkp {
namespace rendering {
namespace controller {

	cloud_controller::cloud_controller(release_renderer* renderer) :
		controller_base(), renderer(renderer), clouds()
	{
		for (int i = 0; i < 10; i++) {

			auto cloud = std::make_shared<model::cloud_model>();
			renderer->add_model(cloud);

			// And its view
			auto cloudView = std::make_shared<view::cloud_view>();
			cloudView->set_model(cloud);
			cloudView->init();
			renderer->add_view(cloudView);

			// And add it to the map
			clouds.insert(std::make_pair(i, cloud));

		}
	}

	cloud_controller::~cloud_controller() {
		//Intentionally left empty
	}

	bool cloud_controller::has_model() const {
		return true;
	}

	void cloud_controller::update(double seconds, std::shared_ptr<abstract_user_input> input) {

		for (int i = 0; i < 10; i++) {
			std::shared_ptr<model::cloud_model> cloud = clouds[i].lock();
			cloud->model_matrix[3][0] = i * 0.3;
			cloud->model_matrix[3][1] = 0.5f;
			cloud->model_matrix[3][2] = i * -0.3;
		}
	}


}
}
}