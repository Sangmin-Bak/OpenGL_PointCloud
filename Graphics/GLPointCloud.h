#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

#include "../PointCloud/TypeDef.h"


namespace GL
{
	namespace Object
	{
		//typedef std::vector<glm::vec3> Cube;
		//typedef std::vector<Cube> PointClouds;

		class GLPointClouds
		{
		public:
			GLPointClouds();
			GLPointClouds(PC::DATA::PointClouds& cloud_in, GLuint cube_width);

			void CreateCube(PC::DATA::PointCloudData& data, std::vector<glm::vec3>& cube_vertex_out, GLfloat& cube_width);
			void CreatePointClouds(PC::DATA::PointClouds& cloud_in, std::vector<std::vector<glm::vec3>>& cloud_gl, GLfloat cube_width);
			GLuint GetSize();

		private:
			std::vector<std::vector<glm::vec3>> m_pointclouds;

			GLuint m_cube_count;
		};
	}
}