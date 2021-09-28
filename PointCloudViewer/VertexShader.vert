#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 vertexColor;

uniform mat4 MVP;
//uniform mat4 M;
// uniform float vertexPointSize;
// uniform int enableVariablePointSize;
// uniform float variablePointSize_K, variablePointSize_DepthScale;

out vec4 frag_color;

void main()
{
	gl_Position = MVP * vec4(position, 1.0);
	//gl_Position = p_matrix * eye_position;

	// Position of the vertex, in worldspace : M * position

	gl_PointSize = 1.0;
	frag_color = vec4(1.0, 1.0, 1.0, 1.0);
}