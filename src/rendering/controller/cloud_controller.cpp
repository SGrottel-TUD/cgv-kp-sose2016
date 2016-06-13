#include "rendering/controller/cloud_controller.hpp"
#include "rendering/controller/sub_cloud_controller.hpp"
#include "rendering/view/cloud_view.hpp"


namespace cgvkp {
namespace rendering {
namespace controller {

	cloud_controller::cloud_controller(release_renderer* renderer, const data::world &data) :
		controller_base(), renderer(renderer), data(data), clouds()
	{
		std::random_device r;
		random_engine = std::default_random_engine(std::seed_seq{ r(), r(), r() });

		w = data.get_config().width();
		h = data.get_config().height();

		int max_clouds = static_cast<int>(h*w)*10;
		int min_sub_clouds = 2;

		for (int i = 0; i < max_clouds; i++) {


			uniform = std::uniform_real_distribution<float>(0.19f, 0.25f);
			float scale = uniform(random_engine);
			auto cloud = std::make_shared<model::cloud_model>(scale);
			cloud->model_matrix[3][1] = 0.5f+scale;
			renderer->add_model(cloud);

			auto cloudView = std::make_shared<view::cloud_view>();
			cloudView->set_model(cloud);
			cloudView->init();
			renderer->add_view(cloudView);

			uniform = std::uniform_real_distribution<float>(-0.01f, 0.01f);
			cloud->speed = 0;//uniform(random_engine);

			uniform = std::uniform_real_distribution<float>(-w, w*2);
			cloud->model_matrix[3][0] = uniform(random_engine);

			cloud->speed = uniform(random_engine)/800;

			cloud->model_matrix[3][2] = 1 + i * -0.15f;

			int add_sub_clouds = rand() % 3;


			float parent_distance_pos = 0;
			float parent_distance_neg = 0;
			for (int j = 0; j < min_sub_clouds+add_sub_clouds; j++) {

				uniform = std::uniform_real_distribution<float>(scale * 0.7, scale * 0.85);
				scale = uniform(random_engine);
				
				if ((rand() % 2) == 0) {
					parent_distance_pos += scale*1.3;
					auto subCloud = std::make_shared<controller::sub_cloud_controller>(renderer, data, cloud, scale, parent_distance_pos);
					renderer->add_controller(subCloud);
				}
				else {
					parent_distance_neg += -scale*1.3;
					auto subCloud = std::make_shared<controller::sub_cloud_controller>(renderer, data, cloud, scale, parent_distance_neg);
					renderer->add_controller(subCloud);
				}

				
				
				
			}			

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

			if (cloud->model_matrix[3][0] > w*2) {
				uniform = std::uniform_real_distribution<float>(-0.0001f, 0.0f);
			} else if (cloud->model_matrix[3][0] < -w) {
				uniform = std::uniform_real_distribution<float>(0.0f, 0.0001f);
			} else {
				uniform = std::uniform_real_distribution<float>(-0.0001f, 0.0001f);
			}
			
			cloud->speed += uniform(random_engine);

			cloud->speed = glm::clamp(cloud->speed, -0.001f, 0.001f);

			cloud->model_matrix[3][0] += cloud->speed;
		}
	}


}
}
}