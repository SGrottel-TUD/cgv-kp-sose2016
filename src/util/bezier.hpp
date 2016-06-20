#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace cgvkp
{
	namespace util
	{
		template<typename T>
		class Bezier
		{
		public:
			Bezier(int degree, std::vector<T> const& curvePoints);
			T sample(float t) const;
			inline float getLength() { return length; }

		private:
			int binomial(int n, int k) const;
			void addVirtPoints(std::vector<T> const& curvePoints);
			void calcT();

			int degree;
			std::vector<T> curvePoints;
			std::vector<float> segmentT;
			float length;
		};

		template<typename T>
		Bezier<T>::Bezier(int _degree, std::vector<T> const& inputPoints)
		{
			degree = _degree;
			addVirtPoints(inputPoints);
			calcT();
		}

		template<typename T>
		T Bezier<T>::sample(float t) const
		{
			int i = 0;
			for (; i < segmentT.size(); i++)
			{
				if (segmentT[i + 1] >= t)
				{
					break;
				}
			}
			float tSeg = (t - segmentT[i]) / (segmentT[i + 1] - segmentT[i]);

			T point;
			for (int k = 0; k <= degree; ++k)
			{
				point += binomial(degree, k) * powf(tSeg, static_cast<float>(k)) * powf(1.0f - tSeg, static_cast<float>(degree - k)) * curvePoints[i * 2 + k];
			}

			return point;
		}

		template<typename T>
		void Bezier<T>::addVirtPoints(std::vector<T> const& inputPoints)
		{
			if (degree == 2)
			{
				int countVirt = 0;
				curvePoints.push_back(inputPoints[0]);
				for (size_t i = 1; i < inputPoints.size() - 2; i++)
				{
					curvePoints.push_back(inputPoints[i]);
					curvePoints.push_back((inputPoints[i] + inputPoints[i + 1]) / 2.0f);
					++countVirt;
				}
				curvePoints.push_back(inputPoints[inputPoints.size() - 2]);
				curvePoints.push_back(inputPoints.back());
			}
			else
			{
				curvePoints = inputPoints;
			}
		}

		template<typename T>
		void Bezier<T>::calcT()
		{
			segmentT.push_back(0);
			length = 0;

			if (degree == 2)
			{
				for (int i = 1; i < curvePoints.size(); i += 2)
				{
					length += glm::length(curvePoints[i] - curvePoints[i - 1]);
					length += glm::length(curvePoints[i + 1] - curvePoints[i]);
					segmentT.push_back(length);
				}
			}
			else
			{
				for (int i = 1; i < curvePoints.size(); i += 2)
				{
					length += glm::length(curvePoints[i] - curvePoints[i - 1]);
					length += glm::length(curvePoints[i + 1] - curvePoints[i]);
				}
			}

			length = segmentT.back();

			for (float& segT : segmentT)
			{
				segT /= segmentT.back();
			}
		}

		template<typename T>
		int Bezier<T>::binomial(int n, int k) const
		{
			if (k == 0 || k == n)
			{
				return 1;
			}
			else
			{
				return binomial(n - 1, k - 1) + binomial(n - 1, k);
			}
		}
	}
}

