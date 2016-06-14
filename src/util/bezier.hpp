#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

namespace cgvkp
{
	namespace rendering
	{
		class Bezier
		{
		public:
			Bezier(bool conic, std::vector<glm::vec2> const & curvePoints);
			void addVirtPoints(std::vector<glm::vec2> const & curvePoints);
			int binomial(int n, int k);
			glm::vec2 sample(float t);
			void calcT();
			inline float getLength() { return length; }

		private:
			std::vector<glm::vec2> curvePoints;
			std::vector<float> segmentT;
			int countPoints;
			int degree;		
			float length;
		};
	}
}

