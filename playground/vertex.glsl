#version 330 core

// inputs: vertex and instance data
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 colorVertex;		// instance
layout(location = 2) in mat4 rotationMatrix;	// instance

uniform mat4 rotationMatrix;

// outputs
out vec3 colorFragment;

void main(){
	gl_Position =  rotationMatrix * vec4(vertex,1);
	colorFragment = colorVertex;
}

