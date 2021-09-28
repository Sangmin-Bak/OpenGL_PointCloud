#pragma once

#include "pch.h"
#include "GLPointCloud.h"

namespace GL
{
	namespace Object
	{
		GLPointClouds::GLPointClouds()
			: m_cube_count(0)
		{

		}

		GLPointClouds::GLPointClouds(PC::DATA::PointClouds& cloud_in, GLuint cube_width)
			: m_pointclouds(cloud_in.size())
			, m_cube_count(cloud_in.size())
		{
			//std::vector<Cube> cube_list(cloud_in.size());
			CreatePointClouds(cloud_in, m_pointclouds, cube_width);
		}

		void GLPointClouds::CreateCube(PC::DATA::PointCloudData& data, std::vector<glm::vec3>& cube_vertex_out, GLfloat& cube_width)
		{
			std::vector<glm::vec3> cube_vertex(8);
			GLfloat len = cube_width / 2.0f;

			cube_vertex[0] = glm::vec3(data.x - len, data.y - len, data.z - len);
			cube_vertex[1] = glm::vec3(data.x + len, data.y - len, data.z - len);
			cube_vertex[2] = glm::vec3(data.x - len, data.y - len, data.z + len);
			cube_vertex[3] = glm::vec3(data.x + len, data.y - len, data.z + len);
			cube_vertex[4] = glm::vec3(data.x - len, data.y + len, data.z - len);
			cube_vertex[5] = glm::vec3(data.x + len, data.y + len, data.z - len);
			cube_vertex[6] = glm::vec3(data.x - len, data.y + len, data.z + len);
			cube_vertex[7] = glm::vec3(data.x + len, data.y + len, data.z + len);

			cube_vertex_out = cube_vertex;
		}

		void GLPointClouds::CreatePointClouds(PC::DATA::PointClouds& cloud_in, std::vector<std::vector<glm::vec3>>& cloud_gl, GLfloat cube_width)
		{
			m_cube_count = cloud_in.size();
			std::vector<std::vector<glm::vec3>> temp(m_cube_count);
			for (int idx = 0; idx < m_cube_count; idx++)
			{
				CreateCube(cloud_in[idx], temp[idx], cube_width);
			}

			cloud_gl = temp;
		}

		GLuint GLPointClouds::GetSize()
		{
			return m_pointclouds.size();
		}
	}
}