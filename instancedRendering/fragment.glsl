#version 330 core

// input: color
in vec3 colorFragment;

// output: color
out vec4 color;

void main()
{
	color = vec4(colorFragment, 1);
}
