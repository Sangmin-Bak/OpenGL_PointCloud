#include "pch.h"
#include "OpenGL.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "../Common/CommonFunction.h"

#define PI	3.141592

GLfloat fov(100.0f);

void MouseClickCallback(GLFWwindow* window, double xpos, double ypos);
void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

namespace GL
{
	OpenGL::OpenGL(GLuint& shader_program, GLuint& vao, CWnd* pImage)
		: m_shaderProgram(shader_program)
		, m_vao(vao)
		, m_fov(45.0f)
		, m_pDC(nullptr)
		, m_view_width(0)
		, m_view_height(0)
		, m_lastX(0.0f)
		, m_lastY(0.0f)
		, m_pitch(0.0f)
		, m_yaw(0.0f)
		, m_roll(0.0f)
		, m_isFirstMove(true)
	{
		//OpenGL context 생성
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

		CRect rc;
		pImage->GetWindowRect(rc);
		m_pDC = pImage->GetDC();

		m_view_width = rc.right - rc.left;
		m_view_height = rc.bottom - rc.top;

		m_viewMatrix = glm::lookAt(
			glm::vec3(0, 0, 100), // 월드 좌표계에서 카메라 위치
			glm::vec3(0, 0, 0), // 카메라가 바라보는 점(초점의 위치)
			glm::vec3(0, 1, 0)  // 카메라 기울임
		);

		//m_modelMatrix = glm::eulerAngleYXZ(-90.f, -30.f, 0.f);

		m_rotMatX = glm::rotate(
			glm::radians(-90.f),
			glm::vec3(1.0f, 0.0f, 0.0f)
		);

		m_rotMatY = glm::rotate(
			glm::radians(-90.f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		m_quaternion = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
	}

	OpenGL::OpenGL(const unsigned short& point_size)
		: m_pointSize(point_size)
		, m_fov(45.0f)
		, m_pDC(nullptr)
		, m_view_width(0)
		, m_view_height(0)
	{
		//OpenGL context 생성
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

		m_rotMatX = glm::rotate(
			glm::radians(-90.f),
			glm::vec3(1.0f, 0.0f, 0.0f)
		);

		m_rotMatY = glm::rotate(
			glm::radians(-90.f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
	}

	OpenGL::~OpenGL()
	{
		if (m_shaderProgram != NULL)
			glDeleteProgram(m_shaderProgram);

		glDeleteVertexArrays(1, &m_vao);
		::wglDeleteContext(m_hRC);
	}

	void OpenGL::GLUseProgram()
	{
		glUseProgram(m_shaderProgram);
		glBindVertexArray(m_vao);
	}

	void OpenGL::GLUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
	{
		glUniformMatrix4fv(m_matrixId, count, transpose, value);
		//glUniformMatrix4fv(m_translateMatID, count, transpose, &m_modelMatrix[0][0]);
	}

	void OpenGL::GLDrawArrays(GLenum mode, GLint first, GLsizei count)
	{
		glDrawArrays(mode, first, count);
	}

	void OpenGL::GLBindVertexArray(GLuint& vao)
	{
		glBindVertexArray(m_vao);
	}

	void OpenGL::GLDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices)
	{
		glDrawElements(mode, count, type, indices);
	}

	GLuint OpenGL::LoadShader(const char* vertex_file_path, const char* fragment_file_path)
	{
		// Create the shaders
		GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		// Read the Vertex Shader code from the file
		std::string VertexShaderCode;
		std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);;

		if (!VertexShaderStream.is_open())
			return GL_FALSE;

		std::stringstream sstr_vertex;
		sstr_vertex << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr_vertex.str();
		VertexShaderStream.close();

		// Read the Fragment Shader code from the file
		std::string FragmentShaderCode;
		std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
		if (!FragmentShaderStream.is_open())
			return GL_FALSE;

		std::stringstream sstr_fragment;
		sstr_fragment << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr_fragment.str();
		FragmentShaderStream.close();

		GLint Result = GL_FALSE;
		int InfoLogLength;

		// vertex shader 초기화 및 compile
		char const* VertexSourcePointer = VertexShaderCode.c_str();
		glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
		glCompileShader(VertexShaderID);
		// shader의 compile 상태 확인
		glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		}

		char const* FragmentSourcePointer = FragmentShaderCode.c_str();
		glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
		glCompileShader(FragmentShaderID);
		// shader의 compile 상태 확인
		glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		}

		// Shader Program 생성 및 link
		GLuint shaderProgram = glCreateProgram();
		// Link the shader program
		glAttachShader(shaderProgram, VertexShaderID);
		glAttachShader(shaderProgram, FragmentShaderID);
		glLinkProgram(shaderProgram);

		// Check the program
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &Result);
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(shaderProgram, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		}

		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);

		return shaderProgram;
	}

	bool OpenGL::GetRenderingContext()
	{
		glfwSetMouseButtonCallback(m_window, (GLFWmousebuttonfun)MouseClickCallback);
		glfwSetScrollCallback(m_window, (GLFWscrollfun)MouseScrollCallback);

		if (m_pDC == NULL)
			return false;

		if (!GetOldStyleRenderingContext())
			return false;

		glewExperimental = GL_TRUE;
		if (GLEW_OK != glewInit())
			return false;

		SetupPixelFormat();

		GLint attribs[] =
		{
			//OpenGL 3.3 사용
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			// Uncomment this for forward compatibility mode
			//WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			// Uncomment this for Compatibility profile
			//WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
			// We are using Core profile here
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};

		HGLRC CompHRC = wglCreateContextAttribsARB(m_pDC->GetSafeHdc(), 0, attribs);
		//HGLRC CompHRC = wglCreateContext(m_pDC->GetSafeHdc());
		if (CompHRC && wglMakeCurrent(m_pDC->GetSafeHdc(), CompHRC))
			m_hRC = CompHRC;

		return true;
	}

	bool OpenGL::GetOldStyleRenderingContext()
	{
		//A generic pixel format descriptor. This will be replaced with a more
		//specific and modern one later, so don't worry about it too much.
		static PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW |            // support window
			PFD_SUPPORT_OPENGL |            // support OpenGL
			PFD_DOUBLEBUFFER,               // double buffered
			PFD_TYPE_RGBA,                  // RGBA type
			32,                             // 32-bit color depth
			0, 0, 0, 0, 0, 0,               // color bits ignored
			0,                              // no alpha buffer
			0,                              // shift bit ignored
			0,                              // no accumulation buffer
			0, 0, 0, 0,                     // accum bits ignored
			24,                        // 24-bit z-buffer
			0,                              // no stencil buffer
			0,                              // no auxiliary buffer
			PFD_MAIN_PLANE,                 // main layer
			0,                              // reserved
			0, 0, 0                         // layer masks ignored
		};

		// Get the id number for the best match supported by the hardware device context
	// to what is described in pfd
		int pixelFormat = ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd);

		//If there's no match, report an error
		if (0 == pixelFormat)
		{
			return false;
		}

		//If there is an acceptable match, set it as the current 
		if (FALSE == SetPixelFormat(m_pDC->GetSafeHdc(), pixelFormat, &pfd))
		{
			return false;
		}

		//Create a context with this pixel format
		if (0 == (m_hRC = wglCreateContext(m_pDC->GetSafeHdc())))
		{
			return false;
		}

		//Make it current. 
		if (FALSE == wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))
		{
			return false;
		}
		return true;
	}

	void OpenGL::SetupPixelFormat()
	{
		//This is a modern pixel format attribute list.
		//It has an extensible structure. Just add in more argument pairs 
		//befroe the null to request more features.
		const int attribList[] =
		{
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			//WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
			WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
			WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB,     24, // 32
			WGL_DEPTH_BITS_ARB,     24,
			WGL_STENCIL_BITS_ARB,   8,
			0  //End
		};

		unsigned int numFormats;
		int pixelFormat;
		PIXELFORMATDESCRIPTOR pfd;

		//Select a pixel format number
		wglChoosePixelFormatARB(m_pDC->GetSafeHdc(), attribList, NULL, 1, &pixelFormat, &numFormats);
	}

	void OpenGL::PointcloudeVAO(PC::DATA::PointClouds& cloud, std::vector<glm::vec3>& cloud_glmVec, glm::mat4& mvp)
	{
		//m_vao = vao;
		glEnable(GL_PROGRAM_POINT_SIZE);

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		GL::Object::GLPointClouds glPointCloud;
		std::vector<glm::vec3> cloud_out(cloud.size());
		//glPointCloud.CreatePointClouds(cloud, cloud_out, 1.0f);
		GL::CommFunc::PointCloud2Vec3(cloud, cloud_out);
		cloud_glmVec = cloud_out;

		GLuint position_vbo, color_vbo;

		glGenBuffers(1, &position_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
		glBufferData(GL_ARRAY_BUFFER, cloud_out.size() * sizeof(glm::vec3), &cloud_out[0], GL_STATIC_DRAW);
		//glBufferData(GL_ARRAY_BUFFER, position.size() * sizeof(glm::vec3), &position[0], GL_STATIC_DRAW);

		m_shaderProgram = LoadShader("VertexShader.vert", "FragmentShader.frag");

		// Get a handle for our "MVP" uniform
		m_matrixId = glGetUniformLocation(m_shaderProgram, "MVP");

		GLint position_attribute = glGetAttribLocation(m_shaderProgram, "position");

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
		glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindVertexArray(0);

		//projection_mat = m_projectionMatrix;
		//view_mat = m_viewMatrix;
		//model_mat = m_modelMatrix;
		mvp = m_MVP;

		glDeleteBuffers(1, &position_vbo);
	}

	CDC* OpenGL::GetWindowDC()
	{
		return m_pDC;
	}

	GLuint& OpenGL::GetShader()
	{
		return m_shaderProgram;
	}

	glm::mat4& OpenGL::GetObjectMVP_Ptr()
	{
		m_projectionMatrix = glm::perspective(glm::radians(fov), (GLfloat)m_view_width / (GLfloat)m_view_height, 0.1f, 500.0f);
		//m_modelMatrix = m_rotMatY * m_rotMatX;// *scale_matrix;
		if (m_quaternion.x == NULL)
			m_modelMatrix = m_rotMatY * m_rotMatX;
		else
			m_modelMatrix = glm::mat4_cast(m_quaternion);

		m_MVP = m_projectionMatrix * m_viewMatrix * m_modelMatrix;
		return m_MVP;
	}

	glm::mat4& OpenGL::GetObjectMVP()
	{
		m_projectionMatrix = glm::perspective(glm::radians(fov), (GLfloat)m_view_width / (GLfloat)m_view_height, 0.1f, 500.0f);

		//glm::mat4 rotation_matrix = glm::rotate(rotation_matrix, glm::radians(45.f), glm::vec3(0.f, 0.f, 1.f));
		//glm::mat4 translate_matrix = glm::translate(translate_matrix, glm::vec3(1.0f));
		//glm::mat4 scale_matrix = glm::scale(scale_matrix, glm::vec3(1.0f, 1.0f, 1.0f));

		if (m_quaternion.x == NULL)
			m_modelMatrix = m_rotMatY * m_rotMatX;
		else
			m_modelMatrix = glm::mat4_cast(m_quaternion);

		m_MVP = m_projectionMatrix * m_viewMatrix * m_modelMatrix;
		return m_MVP;
	}

	void OpenGL::RotateObject()
	{
		m_projectionMatrix = glm::perspective(glm::radians(fov), (GLfloat)m_view_width / (GLfloat)m_view_height, 0.1f, 500.0f);

		glm::quat model_quat = glm::quat_cast(m_modelMatrix);

		glm::quat quaternion = Quaternion(45.0f);
		m_modelMatrix = glm::mat4_cast(quaternion);
		m_MVP = m_projectionMatrix * m_viewMatrix * m_modelMatrix;
	}

	void OpenGL::TransposeShader(std::vector<glm::vec3>& data)
	{
		if (data.size() == 0)
		{
			return;
		}

		GLuint position_vbo, color_vbo;
		glGenBuffers(1, &position_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec3), &data[0], GL_STATIC_DRAW);

		m_shaderProgram = LoadShader("VertexShader.vert", "FragmentShader.frag");

		m_matrixId = glGetUniformLocation(m_shaderProgram, "MVP");
		m_translateMatID = glGetUniformLocation(m_shaderProgram, "M");

		GLint position_attribute = glGetAttribLocation(m_shaderProgram, "position");

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
		glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindVertexArray(0);

		glUniformMatrix4fv(m_matrixId, 1, GL_FALSE, &m_MVP[0][0]);
		//glUniformMatrix4fv(m_translateMatID, 1, GL_FALSE, &m_projectionMatrix[0][0]);
	}

	glm::quat OpenGL::Quaternion(GLfloat angle, glm::vec3 axis)
	{
		GLfloat angle_radian = glm::radians(angle);

		GLfloat sin_theta = sin(angle_radian / 2.0);
		GLfloat cos_theta = cos(angle_radian / 2.0);

		GLfloat rx = axis.x * sin_theta;
		GLfloat ry = axis.y * sin_theta;
		GLfloat rz = axis.z * sin_theta;
		GLfloat rw = cos_theta;

		glm::quat quaternion(rw, rx, ry, rz);
		return quaternion;
	}

	void OpenGL::SetGLMatrix(const glm::mat4& projection_mat, const glm::mat4& view_mat, const glm::mat4& model_mat)
	{
		m_projectionMatrix = projection_mat;
		m_viewMatrix = view_mat;
		m_modelMatrix = model_mat;
	}

	void OpenGL::MouseWheelEvent(GLshort& zDelta)
	{
		GLfloat offset = static_cast<GLfloat>(zDelta) / 36.0f;
		fov -= offset;
		if (fov < 12.0f)
			fov = 12.0f;
		if (fov > 180.0f)
			fov = 180.0f;
	}

	void OpenGL::RotateObject(GLfloat pos_x, GLfloat pos_y)
	{
		if (m_isFirstMove)
		{
			m_lastX = pos_x;
			m_lastY = pos_y;
			m_isFirstMove = false;
		}

		GLfloat xoffset = pos_x - m_lastX;
		GLfloat yoffset = m_lastY - pos_y;
		m_lastX = pos_x;
		m_lastY = pos_y;

		GLfloat sensitivity = 0.02f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		m_yaw += xoffset;
		m_pitch -= yoffset;

		if (m_pitch > 89.0f)
			m_pitch = 89.0f;
		if (m_pitch < -89.0f)
			m_pitch = -89.0f;

		m_quaternion = glm::quat(glm::vec3(m_pitch, m_yaw, m_roll));
	}
}

void MouseClickCallback(GLFWwindow* window, double xpos, double ypos)
{
	return;
}

void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}