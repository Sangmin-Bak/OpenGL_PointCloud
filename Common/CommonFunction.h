#pragma once

#include "../PointCloud/TypeDef.h"

#include <GL/glew.h>
#include <GL/wglew.h>
#include <glm/glm.hpp>
#include <vector>

namespace GL
{
	namespace CommFunc
	{
		void PointCloud2Vec3(PC::DATA::PointClouds& pClouds, std::vector<glm::vec3>& glVector);
		float GetRadianAngle(float x_left, float y_left, float x_right, float y_right);
		float GetDegreeAngle(float x_left, float y_left, float x_right, float y_right);
	}
}