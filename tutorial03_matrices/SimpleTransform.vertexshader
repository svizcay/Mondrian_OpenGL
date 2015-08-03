#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;
layout(location = 1) in vec4 inputColor;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
out vec4 fragmentColor;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	// gl_Position =  vertexPosition_modelspace;
	gl_Position =  MVP * vertexPosition_modelspace;
	fragmentColor = inputColor;
}

