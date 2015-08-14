#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;
layout(location = 1) in vec4 inputColor;

// TODO: try to transfer just one mat4
layout(location = 2) in vec4 MVProw1;
layout(location = 3) in vec4 MVProw2;
layout(location = 4) in vec4 MVProw3;
layout(location = 5) in vec4 MVProw4;

// TODO: try to transfer just one mat4
layout(location = 2) in vec4 MVProw1;
layout(location = 3) in vec4 MVProw2;
layout(location = 4) in vec4 MVProw3;
layout(location = 5) in vec4 MVProw4;

// Values that stay constant for the whole mesh.
// uniform mat4 MVP;
out vec4 fragmentColor;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	// gl_Position =  vertexPosition_modelspace;
	mat4 MVP = mat4(MVProw1, MVProw2, MVProw3, MVProw4);
	// gl_Position =  vec4((MVP * vertexPosition_modelspace).xy, 0, 1);
	// gl_Position =  vertexPosition_modelspace;
	gl_Position =  MVP * vertexPosition_modelspace;
	// gl_Position =  vertexPosition_modelspace;
	fragmentColor = inputColor;
}

