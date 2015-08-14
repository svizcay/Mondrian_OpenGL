#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec2 position;

// Values that stay constant for the whole mesh.
// out vec4 fragmentColor;

// uniform mat4 MVP;

void main()
{
	gl_Position =  vec4(position, 0, 1);
}
