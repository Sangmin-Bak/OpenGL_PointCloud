#pragma once

#include "pch.h"
#include "CommonFunction.h"


namespace GL
{
	namespace CommFunc
	{
		void PointCloud2Vec3(PC::DATA::PointClouds& pClouds, std::vector<glm::vec3>& glVector)
		{
			if (glVector.size() == 0)
			{
				for (int idx = 0; idx < pClouds.size(); idx++)
				{
					glVector.push_back(glm::vec3(pClouds[idx].x, pClouds[idx].y, pClouds[idx].z));
				}
			}
			else
			{
				for (int idx = 0; idx < pClouds.size(); idx++)
				{
					glVector[idx] = glm::vec3(pClouds[idx].x, pClouds[idx].y, pClouds[idx].z);
				}
			}
		}

		float GetRadianAngle(float x_left, float y_left, float x_right, float y_right)
		{
			float width = x_right - x_left;
			float height = y_right - y_left;

			// atan2 함수는 라디안 값을 반환
			//float theta_degree = atan2f(height, width) * 180.f / 3.141592f;
			float theta_radian = atan2f(height, width);
			return theta_radian;
		}

		float GetDegreeAngle(float x_left, float y_left, float x_right, float y_right)
		{
			float width = x_right - x_left;
			float height = y_right - y_left;

			// atan2 함수는 라디안 값을 반환
			float theta_degree = atan2f(height, width) * 180.f / 3.141592f;
			return theta_degree;
		}
	}
}