#include "rendering/controller/sub_cloud_controller.hpp"

#include "rendering/view/cloud_view.hpp"


namespace cgvkp {
	namespace rendering {
		namespace controller {

			sub_cloud_controller::sub_cloud_controller(release_renderer* renderer, const data::world &data, std::shared_ptr<model::cloud_model> parent_cloud, float scale, float parent_distance) :
				controller_base(), renderer(renderer), data(data), parent_cloud(parent_cloud), parent_distance(parent_distance)
			{

				std::random_device r;
				random_engine = std::default_random_engine(std::seed_seq{ r(), r(), r() });
				
				auto subCloud = std::make_shared<model::cloud_model>(scale);
				subCloud->model_matrix[3].y = scale;
				renderer->add_model(subCloud);

				auto cloudView = std::make_shared<view::cloud_view>();
				cloudView->set_model(subCloud);
				cloudView->init();
				renderer->add_view(cloudView);

				parent_x = parent_cloud->model_matrix[3].x;
				float parent_z = parent_cloud->model_matrix[3].z;

				uniform = std::uniform_real_distribution<float>(-0.4f,0.4f);
				//parent_distance = uniform(random_engine);
				subCloud->model_matrix[3].x = parent_x + parent_distance;

				if ((rand() % 2) != 0) {
					scale = -scale;
				}
				subCloud->model_matrix[3].z = parent_z+scale;

				scale = abs(scale);
				uniform = std::uniform_real_distribution<float>(0.7f * scale, 0.85f * scale);
				scale = uniform(random_engine);

				int parent_sgn = (parent_distance > 0) - (parent_distance < 0);

				if ((rand() % 2) != 0) {
					auto subSubCloud = std::make_shared<controller::sub_cloud_controller>(renderer, data, subCloud, scale, scale*1.3f*parent_sgn);
					renderer->add_controller(subSubCloud);
				}

				sub_cloud = std::move(subCloud);

			}
			

			sub_cloud_controller::~sub_cloud_controller() {
				//Intentionally left empty
			}

			bool sub_cloud_controller::has_model() const {
				return true;
			}

			void sub_cloud_controller::update(double seconds, std::shared_ptr<abstract_user_input> input) {
				parent_x = parent_cloud->model_matrix[3].x;
				float diff = parent_x + parent_distance - sub_cloud->model_matrix[3].x;
				sub_cloud->model_matrix[3].x += std::pow(diff,1)*0.1f;
			}


		}
	}
}