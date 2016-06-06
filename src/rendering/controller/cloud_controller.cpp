#include "rendering/controller/cloud_controller.hpp"

#include "rendering/view/cloud_view.hpp"


namespace cgvkp {
namespace rendering {
namespace controller {

	cloud_controller::cloud_controller(release_renderer* renderer, const data::world &data) :
		controller_base(), renderer(renderer), data(data), clouds()
	{
		std::random_device r;
		random_engine = std::default_random_engine(std::seed_seq{ r(), r(), r() });
		uniform = std::uniform_real_distribution<float>(0.0f, 1.0f);

		float w = data.get_config().width();
		float h = data.get_config().height();

		int max_clouds = static_cast<int>(h*w)*5;

		for (int i = 0; i < max_clouds; i++) {

			auto cloud = std::make_shared<model::cloud_model>();
			renderer->add_model(cloud);

			auto cloudView = std::make_shared<view::cloud_view>();
			cloudView->set_model(cloud);
			cloudView->init();
			renderer->add_view(cloudView);

			uniform = std::uniform_real_distribution<float>(-0.01f, 0.01f);
			cloud->speed = 0;//uniform(random_engine);

			uniform = std::uniform_real_distribution<float>(-w, w*2);
			cloud->model_matrix[3][0] = uniform(random_engine);

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

		for (int i = 0; i < clouds.size(); i++) {
			std::shared_ptr<model::cloud_model> cloud = clouds[i].lock();

			uniform = std::uniform_real_distribution<float>(-0.0001f, 0.0001);
			cloud->speed += uniform(random_engine);

			cloud->model_matrix[3][0] += cloud->speed;
			cloud->model_matrix[3][1] = 0.5f;
			cloud->model_matrix[3][2] = i * -0.3f;
		}
	}


}
}
}