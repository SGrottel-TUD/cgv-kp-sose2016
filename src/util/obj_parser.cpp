#include "util/obj_parser.hpp"
#include "util/resource_file.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <glm/glm.hpp>

namespace cgvkp {
	namespace util {
		bool obj_parser::refactor_vector(
			  std::vector<uint32_t> &index,
			  std::vector<float> &data,
			  unsigned int data_size,
			  std::vector<float> &out
			) {
			// Ensure we got sane values
			if (index.size() == 0)
				return false;
			// Find amount of data by calculating max value on vertex_index
			unsigned int index_count = 1 + *std::max_element(index.begin(), index.end());
			// Ensure we have enough information in data.
			if (data_size * index_count > data.size())
				return false;
			// Initialize vector with enough space
			out.clear();
			out.reserve(data_size * index.size());
			// Iterate through index list
			for (unsigned int i = 0u; i < index.size(); ++i) {
				// Copy data to out vector
				for (unsigned int j = 0u; j < data_size; ++j)
					out.push_back(data[data_size * index[i] + j]);
			}
			return true;
		}
		bool obj_parser::average_vertex_normals(
				std::vector<uint32_t> &vertex_index,
				std::vector<uint32_t> &normal_index,
				std::vector<float> &normals,
				std::vector<float> &out) {
			// Ensure we got sane values
			if (vertex_index.size() == 0 ||
				vertex_index.size() != normal_index.size())
				return false;
			// Find amount of vertex by calculating max value on vertex_index
			unsigned int vertex_count = 1 + *std::max_element(vertex_index.begin(), vertex_index.end());
			unsigned int normal_count = 1 + *std::max_element(normal_index.begin(), normal_index.end());
			// Ensure we have enough information in normals.
			if (3 * normal_count > normals.size())
				return false;
			out.clear();
			// Reserve space accordingly (normal needs 3 coordinates)
			// And initialize everything to zero
			out.assign(vertex_count * 3, 0.0f);

			// Add all normals for each vertex
			for (unsigned int i = 0; i < normal_index.size(); ++i) {
				unsigned int n_i = normal_index[i];
				unsigned int v_i = vertex_index[i];
				// Add in out
				for (unsigned int j = 0; j < 3; ++j)
					out[3 * v_i + j] += normals[3 * n_i + j];
			}
			// Normalize each normal if possible (if not zero)
			for (unsigned int v_i = 0; v_i < vertex_count; ++v_i) {
				// Calculate vector length
				float len = 0.0f;
				for (unsigned int j = 0; j < 3; ++j)
					len += powf(out[3 * v_i + j], 2.0f);
				len = sqrtf(len);
				// Normalize
				if (len > 0.0f) {
					for (unsigned int j = 0; j < 3; ++j)
						out[3 * v_i + j] /= len;
				}
			}
			return true;
	};
		bool obj_parser::parse(
				const char *name,
				std::vector<float> &vertex_array,
				std::vector<float> &normal_array
				) {
			std::vector<float> indexed_vertex;
			std::vector<float> indexed_normals;
			std::vector<uint32_t> normal_indices;
			std::vector<uint32_t> index;
			// Parse obj file
			if (!util::obj_parser::do_parse(name, indexed_vertex, indexed_normals, index, normal_indices)) {
				std::cerr << "Failed to read '" << name << "'." << std::endl;
				return false;
			}
			// Refactor data
			if (!util::obj_parser::refactor_vector(index, indexed_vertex, 3, vertex_array)) {
				std::cerr << "Error refactoring vertex coordinates for '" << name << "'." << std::endl;
				return false;
			}
			std::vector<float> temp_normals;
			if (//!util::obj_parser::refactor_vector(normal_indices, indexed_normals, 3, normal_array)
				!util::obj_parser::average_vertex_normals(index, normal_indices, indexed_normals, temp_normals)
				|| !util::obj_parser::refactor_vector(index, temp_normals, 3, normal_array)
				) {
				std::cerr << "Error refactoring normals for '" << name << "'." << std::endl;
				return false;
			}
			assert(vertex_array.size() == normal_array.size());
			assert(vertex_array.size() % 3 == 0);
			return true;
		}
		bool obj_parser::do_parse(
				const char *name,
				std::vector<float> &vertex,
				std::vector<float> &normal,
				std::vector<uint32_t> &index,
				std::vector<uint32_t> &normal_index) {
			std::string fn = resource_file::find_resource_file(name);
			if (fn.empty()) return false;
			// Open file
			std::ifstream file;
			file.open(fn, std::ifstream::in);
			// Calculate file size in bytes
			std::streampos fsize = file.tellg();
			file.seekg(0, std::ios::end);
			fsize = file.tellg() - fsize;
			// Go back to the beginning
			file.seekg(0, std::ios::beg);
			// Reserve space estimating 30 chars / line and 50% facets
			vertex.reserve(3 * static_cast<unsigned int>(0.25f * fsize / 30));
			normal.reserve(3 * static_cast<unsigned int>(0.25f * fsize / 30));
			index.reserve(3 * static_cast<unsigned int>(0.50f * fsize / 30));
			normal_index.reserve(3 * static_cast<unsigned int>(0.50f * fsize / 30));
			// Start parsing
			// Cur_pos for groups
			int g_v(0), g_n(0);
			for (std::string line;  std::getline(file, line); ) {
				// Ignore everything but "v ", "vn" and "f " lines
				// TODO: accept "vt"

				// Make stream for the line
				std::stringstream line_stream(line);
				std::string b_id;
				// Read start
				line_stream >> b_id;
				if (b_id == "gaaa") {
					// Save position
					g_v = vertex.size();
					g_n = normal.size() / 3;
				}
				if (b_id == "v") {
					// Vertex coordinates
					float x, y, z;
					line_stream >> x >> y >> z;
					vertex.push_back(x);
					vertex.push_back(y);
					vertex.push_back(z);
				}
				if (b_id == "vn") {
					// Normal coordinates
					float x, y, z;
					line_stream >> x >> y >> z;
					normal.push_back(x);
					normal.push_back(y);
					normal.push_back(z);
				}
				if (b_id == "f") {
					// Facet
					std::string part;
					int vs[3], ns[3], ts[3];
					// Read data
					for (int i = 0; i < 3; ++i) {
						line_stream >> part;
						int vi(-1), ti(-1), ni(-1);
						if (3 != sscanf_s(part.data(), "%d/%d/%d", &vi, &ti, &ni)) {
							if (2 != sscanf_s(part.data(), "%d//%d", &vi, &ni)) {
								if (2 != sscanf_s(part.data(), "%d/%d", &vi, &ti)) {
									file.close();
									return false;
								}
							}
						}
						vs[i] = --vi;
						ns[i] = --ni;
						ts[i] = --ti;
						if (vs[i] < 0) {
							file.close();
							return false;
						}
						index.push_back(vs[i] + g_v); // Compensate already read groups
					}
					// Deal with missing data
					if (ns[0] < 0 || ns[1] < 0 || ns[2] < 0) {
						// No normal was given, calculate it
						glm::vec3 ps[3];
						// Read points
						for (unsigned int i = 0u; i < 3u; ++i) {
							unsigned int j = 3u * vs[i];
							ps[i] = glm::vec3(vertex[j], vertex[j + 1], vertex[j + 2]);
						}
						// Calculate cross product
						glm::vec3 n = glm::normalize(glm::cross(ps[1] - ps[0], ps[2] - ps[0]));
						// Assign index
						ns[0] = normal.size() / 3 - g_n; // Compensate already read groups
						ns[1] = ns[0];
						ns[2] = ns[0];
						// Add data to normals
						normal.push_back(n.x);
						normal.push_back(n.y);
						normal.push_back(n.z);
					}
					// Add normal index
					for (unsigned int i = 0u; i < 3u; ++i)
						normal_index.push_back(ns[i] + g_n); // Compensate already read groups
				}
			}
			// Finish
			file.close();
			// Normalize to a [-1, 1] space
			auto minmax = std::minmax_element(vertex.begin(), vertex.end());
			float M = std::fmaxf(std::abs(*(minmax.first)), std::abs(*(minmax.second)));
			for (float &v : vertex) {
				v /= M;
			}
			return true;
		}

	}
}