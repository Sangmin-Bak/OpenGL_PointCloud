#pragma once

#ifdef _GRAPHICS_
#define GRAPHICS_DECLSPEC _declspec(dllexport)
#else
#define GRAPHICS_DECLSPEC _declspec(dllimport)
#endif

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../PointCloud/TypeDef.h"
#include "GLPointCloud.h"


namespace GL
{
	class GRAPHICS_DECLSPEC OpenGL
	{
	public:
		OpenGL(GLuint& shader_program, GLuint& vao, CWnd* pImage);
		OpenGL(const unsigned short& point_size);
		~OpenGL();

		void GLUseProgram();
		void GLUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
		void GLBindVertexArray(GLuint& vao);
		void GLDrawArrays(GLenum mode, GLint first, GLsizei count);
		void GLDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices);

		bool GetRenderingContext();
		bool GetOldStyleRenderingContext();
		void SetupPixelFormat();
		void PointcloudeVAO(PC::DATA::PointClouds& cloud, std::vector<glm::vec3>& cloud_glmVec, glm::mat4& mvpo);
		GLuint LoadShader(const char* vertex_file_path, const char* fragment_file_path);

		CDC* GetWindowDC();
		GLuint& GetShader();
		glm::mat4& GetObjectMVP_Ptr();
		void RotateObject();

		void SetGLMatrix(const glm::mat4& projection_mat, const glm::mat4& view_mat, const glm::mat4& model_mat);

		void MouseWheelEvent(GLshort& zDelta);
		void RotateObject(GLfloat pos_x, GLfloat pos_y);

		glm::mat4& GetObjectMVP();

		void TransposeShader(std::vector<glm::vec3>& data);

		glm::quat Quaternion(GLfloat angle, glm::vec3 axis = glm::vec3(1.0f, 1.0f, 0.0f));

		//static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		//void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	private:
		unsigned short m_pointSize;
		bool m_isFirstMove;

		GLfloat m_fov;

		GLFWwindow* m_window;

		HGLRC m_hRC;
		CDC* m_pDC;

		GLuint m_view_width;
		GLuint m_view_height;

		GLfloat m_lastX;
		GLfloat m_lastY;

		GLfloat m_pitch;
		GLfloat m_yaw;
		GLfloat m_roll;
		glm::quat m_quaternion;

		GLuint m_vao;
		GLuint m_shaderProgram;
		GLuint m_matrixId;
		GLuint m_translateMatID;
		glm::mat4 m_projectionMatrix;
		glm::mat4 m_viewMatrix;
		glm::mat4 m_modelMatrix;
		glm::mat4 m_MVP;

		glm::mat4 m_rotMatX;
		glm::mat4 m_rotMatY;
	};
}
