#version 330 core

in vec4 fragmentColor;

// Ouput data
out vec4 color;

void main()
{

	// Output color = green 
	color = fragmentColor;
	// color = vec4(1, 0, 0, 1);

}
