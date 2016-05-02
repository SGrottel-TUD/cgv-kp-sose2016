#pragma once
#include <string>
#include <vector>
#include <memory>

namespace cgvkp {
	namespace util {

		class obj_parser {
		public:
			static bool parse(
				const char *name,
				std::vector<float> &vertex_array,
				std::vector<float> &normals
				);
			// Refactor vector without applying any kind of normalization
			static bool obj_parser::refactor_vector(
				std::vector<uint32_t> &index,
				std::vector<float> &data,
				unsigned int data_size,
				std::vector<float> &out);
			// Assume a smooth object, vertex normals will be averaged
			// In out, normals are ordered according to vertex_index
			static bool average_vertex_normals(
				std::vector<uint32_t> &vertex_index,
				std::vector<uint32_t> &normal_index,
				std::vector<float> &normals,
				std::vector<float> &out);
			static bool do_parse(
				const char *name,
				std::vector<float> &vertex,
				std::vector<float> &normal,
				std::vector<uint32_t> &index,
				std::vector<uint32_t> &normal_index);
		private:
			obj_parser() = delete;
			obj_parser(const obj_parser &that) = delete;
		};
	}
}
