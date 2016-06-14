#include "bezier.hpp"



cgvkp::rendering::Bezier::Bezier(bool conic, std::vector<glm::vec2> const & inputPoints)
{
	countPoints = static_cast<int>(inputPoints.size());
	conic ? degree = 2 : degree = 3;

	addVirtPoints(inputPoints);
	calcT();
}

void cgvkp::rendering::Bezier::addVirtPoints(std::vector<glm::vec2> const & inputpoints)
{
	
	int countVirt = 0;
	if (degree == 2)
	{
		curvePoints.push_back(inputpoints[0]);
		for (int i = 1; i < countPoints - 2; i++)
		{
			curvePoints.push_back(inputpoints[i]);
			curvePoints.push_back((inputpoints[i] + inputpoints[i + 1]) / 2.0f);
			countVirt += 1;
		}
		curvePoints.push_back(inputpoints[countPoints - 2]);
		curvePoints.push_back(inputpoints[countPoints - 1]);
		countPoints += countVirt;
		return;
	}
	curvePoints = inputpoints;
}


void cgvkp::rendering::Bezier::calcT()
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

	for (int i = 0; i < segmentT.size(); i++)
	{
		segmentT[i] /= segmentT.back();
	}
}

glm::vec2 cgvkp::rendering::Bezier::sample(float t)
{
	glm::vec2 point;
		int i = 0;
		for (; i < segmentT.size(); i++)
		{
			if (segmentT[i + 1] >= t)
			{
				break;
			}
		}
		float tseg = (t - segmentT[i]) / (segmentT[i + 1] - segmentT[i]);

		int start = i * 2;
		for (int k = 0; k <= degree; k++)
		{
			point += binomial(degree, k) * powf(tseg, static_cast<float>(k)) * powf(1.0f - tseg, static_cast<float>(degree - k)) * curvePoints[i * 2 + k];
		}
	
	return point;
}

int  cgvkp::rendering::Bezier::binomial(int n, int k)
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




