#pragma once

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
			inline float getLength() const { return length; }

		private:
			int binomial(int n, int k) const;

			int degree;
			std::vector<T> curvePoints;
			std::vector<float> segmentT;
			float length;
		};

		template<typename T>
		Bezier<T>::Bezier(int _degree, std::vector<T> const& inputPoints)
			: length(0)
		{
			degree = _degree;

			if (degree < 2)
			{
				throw std::exception("Degree must be greater than 1.");
			}
			if ((inputPoints.size() - 2) % (degree - 1))
			{
				throw std::exception("Only a multiple number of (degree - 1) control points are allowed.");
			}

			// Calculate virutal curve points if necessary.
			curvePoints.push_back(inputPoints.front());
			for (size_t i = degree; i < inputPoints.size() - 1; i += degree - 1)
			{
				for (int k = degree - 1; k > 0; --k)
				{
					curvePoints.push_back(inputPoints[i - k]);
				}
				curvePoints.push_back((inputPoints[i - 1] + inputPoints[i]) / 2.0f);
			}
			for (int i = degree - 1; i > 0; --i)
			{
				curvePoints.push_back(inputPoints[inputPoints.size() - 1 - i]);
			}
			curvePoints.push_back(inputPoints.back());

			// Calculate the global ts for the segments.
			segmentT.push_back(0);
			for (size_t i = degree; i < curvePoints.size(); i += degree)
			{
				for (int k = 0; k < degree; ++k)
				{
					length += glm::length(curvePoints[i - k] - curvePoints[i - k - 1]);
				}
				segmentT.push_back(length);
			}

			length = segmentT.back();
			for (auto& segT : segmentT)
			{
				segT /= length;
			}
		}

		template<typename T>
		T Bezier<T>::sample(float t) const
		{
			int i = 0;
			for (; i < segmentT.size(); ++i)
			{
				if (segmentT[i + 1] >= t)
				{
					break;
				}
			}
			float tSeg = (t - segmentT[i]) / (segmentT[i + 1] - segmentT[i]);

			T point(0);
			for (int k = 0; k <= degree; ++k)
			{
				point += binomial(degree, k) * powf(tSeg, static_cast<float>(k)) * powf(1.0f - tSeg, static_cast<float>(degree - k)) * curvePoints[i * degree + k];
			}

			return point;
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

