#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec2 position;

// Values that stay constant for the whole mesh.
// uniform mat4 MVP;
// out vec4 fragmentColor;

void main()
{
	gl_Position =  vec4(position, 0, 1);
}
