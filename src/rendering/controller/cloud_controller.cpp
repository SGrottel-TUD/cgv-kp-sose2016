#include "rendering/controller/cloud_controller.hpp"
#include "rendering/controller/sub_cloud_controller.hpp"
#include "rendering/view/cloud_view.hpp"
#include "util/bezier.hpp"


namespace cgvkp {
namespace rendering {
namespace controller {

	cloud_controller::cloud_controller(release_renderer* renderer, const data::world &data, Mesh const& mesh) :
		controller_base(), renderer(renderer), data(data), clouds()
	{
		std::random_device r;
		random_engine = std::default_random_engine(std::seed_seq{ r(), r(), r() });

		int_uniform = std::uniform_int_distribution<int>(150, 300);

		w = data.get_config().width();
		h = data.get_config().height();

		int max_clouds = static_cast<int>(h*w)*6;

		for (int i = 0; i < max_clouds; i++) {


			uniform = std::uniform_real_distribution<float>(0.19f, 0.22f);
			float scale = uniform(random_engine);
			auto cloud = std::make_shared<model::cloud_model>(scale);
			cloud->model_matrix[3].y = scale;
			renderer->add_model(cloud);

			auto cloudView = std::make_shared<view::cloud_view>(mesh);
			cloudView->set_model(cloud);
			renderer->add_view(cloudView);

			uniform = std::uniform_real_distribution<float>(-0.01f, 0.01f);
			cloud->speed = uniform(random_engine);

			uniform = std::uniform_real_distribution<float>(0, w);
			cloud->model_matrix[3].x = uniform(random_engine);

			cloud->speed = uniform(random_engine)/800;

			cloud->model_matrix[3].z = -i * h / max_clouds;




			uniform = std::uniform_real_distribution<float>(scale * 0.7f, scale * 0.85f);
			scale = uniform(random_engine);
				
			auto subCloud1 = std::make_shared<controller::sub_cloud_controller>(renderer, data, mesh, cloud, scale, scale*1.3f);
			renderer->add_controller(subCloud1);

			scale = uniform(random_engine);
			
			auto subCloud2 = std::make_shared<controller::sub_cloud_controller>(renderer, data, mesh, cloud, scale, -scale*1.3f);
			renderer->add_controller(subCloud2);
			

			cloud->speed_curve = calculate_new_speed_curve();
			cloud->curve_iterator = 0;
				
				
						

			clouds.insert(std::make_pair(i, cloud));

		}
	}

	cloud_controller::~cloud_controller() {
		//Intentionally left empty
	}

	bool cloud_controller::has_model() const {
		return true;
	}

	float cloud_controller::calculate_max_cloud_space(float z) {
		float wMax = static_cast<float>((h + renderer->getDistance())*((w+0.5)/(renderer->getAspect())));
		return -z*(wMax - w) / (4 * h);
	}

	std::vector<float> cloud_controller::calculate_new_speed_curve() {
		int curve_points = 4;
		std::vector<float> control_points;

		uniform = std::uniform_real_distribution<float>(-1.f, 1.f);

		for (int i = 0; i < curve_points; i++) {
			control_points.push_back(uniform(random_engine));
		}

		util::Bezier<float> bezier(3, control_points);
		int samples = int_uniform(random_engine);
		std::vector<float> new_curve;

		for (int i = 0; i < samples; i++) {
			new_curve.push_back(bezier.sample(i*(1.f / samples)));
		}

		return new_curve;

	}

	void cloud_controller::update(double seconds, std::shared_ptr<abstract_user_input> input) {

		for (int i = 0; i < clouds.size(); i++) {
			std::shared_ptr<model::cloud_model> cloud = clouds[i].lock();
			
			if (cloud->model_matrix[3].x > w+calculate_max_cloud_space(cloud->model_matrix[3].z)) {
				cloud->speed = -0.001f;
			} else if (cloud->model_matrix[3].x < -calculate_max_cloud_space(cloud->model_matrix[3].z)) {
				cloud->speed = 0.001f;
			} else {
				if (cloud->curve_iterator >= cloud->speed_curve.size()) {
					cloud->speed_curve = calculate_new_speed_curve();
					cloud->curve_iterator = 0;
				}
				cloud->speed += (cloud->speed_curve[cloud->curve_iterator])*0.001f;
			}
			cloud->curve_iterator++;

				

			cloud->speed = glm::clamp(cloud->speed, -0.001f, 0.001f);


			for (auto data_hand : data.get_hands()) {
				glm::vec3 hand_position = glm::vec3(data_hand->x, trans_height(data_hand->height), -data_hand->y);
				auto hand_to_cloud = glm::vec3(cloud->model_matrix[3]) - hand_position;
				auto distance = glm::length(hand_to_cloud)*1.2;
				cloud->speed += (cloud->speed)*(1 / std::pow(distance,2));

			}

			cloud->speed = glm::clamp(cloud->speed, -0.012f, 0.012f);


			cloud->model_matrix[3].x += cloud->speed;

		}
	}


}
}
}