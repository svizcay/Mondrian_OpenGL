// Include standard headers
#include <iostream>
#include <cstdlib>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Include SOIL: Simple OpenGL Image Library
#include <SOIL/SOIL.h>

#include <common/shader.hpp>

int main( void )
{
	// Initialise GLFW
	if ( !glfwInit() ) {
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(
			GLFW_OPENGL_PROFILE,
			GLFW_OPENGL_CORE_PROFILE
	);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(
			640, 480,
			"Tutorial 03 - Matrices",
			NULL, NULL
	);
	if ( window == NULL ) {
		std::cerr << "Failed to open GLFW window. ";
		std::cerr << "If you have an Intel GPU, ";
		std::cerr << "they are not 3.3 compatible. ";
		std::cerr << "Try the 2.1 version of the tutorials.";
		std::cerr << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// glViewport(0, 0, 640, 480);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders(
			"vertex.glsl",
			"fragment.glsl"
	);
	glUseProgram(programID);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glm::mat4 rotationMatrix1 = glm::rotate(
			glm::mat4(1), 0.0f, glm::vec3(0, 0, 1)
	);
	glm::mat4 rotationMatrix2 = glm::rotate(
			glm::mat4(1), 180.0f, glm::vec3(0, 0, 1)
	);

	glm::mat4 rotationMatrices[2] = {
		rotationMatrix1,
		rotationMatrix2
	};

	static const GLfloat triangleVertices[] = {
		-1.0f, -1.0f, 0.0f,	// vertex a bottom left
		 1.0f, -1.0f, 0.0f,	// vertex b bottom right
		 1.0f,  1.0f, 0.0f,	// vertex c top right
	};
	static const GLfloat triangleColors[] = {
		1.0f,	0.0f,	0.0f,	// red
		0.0f,	1.0f,	0.0f,	// green
	};

	/*
	 * VBOs
	 */

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(
			GL_ARRAY_BUFFER,
			sizeof(triangleVertices),
			triangleVertices, GL_STATIC_DRAW
	);
	GLint vertexLocation = glGetAttribLocation(
			programID, "vertex"
	);
	glEnableVertexAttribArray(vertexLocation);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		vertexLocation,		// attribute index
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	GLuint colorBuffer;
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(
			GL_ARRAY_BUFFER,
			sizeof(triangleColors),
			triangleColors, GL_STATIC_DRAW
	);
	GLint colorLocation = glGetAttribLocation(
			programID, "colorVertex"
	);
	glEnableVertexAttribArray(colorLocation);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glVertexAttribPointer(
		colorLocation,      // attribute location
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glVertexAttribDivisor(colorLocation, 1);

	GLuint rotationMatrixBuffer;
	glGenBuffers(1, &rotationMatrixBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, rotationMatrixBuffer);
	glBufferData(
			GL_ARRAY_BUFFER,
			sizeof(rotationMatrices),
			rotationMatrices, GL_STATIC_DRAW
	);
	GLint rotationMatrixLocation = glGetAttribLocation(programID, "rotationMatrix");

	for (unsigned i = 0; i < 4; i++) {
		// set up vertex attribute
		glVertexAttribPointer(
				rotationMatrixLocation+i,	// attrib index
				4,							// nr
				GL_FLOAT,
				GL_FALSE,
				sizeof(glm::mat4),
				(void *)(sizeof(glm::vec4) *i)
		);
		glEnableVertexAttribArray(rotationMatrixLocation+i);
		glVertexAttribDivisor(rotationMatrixLocation+i, 1);
	}

	do{

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glBindVertexArray(VertexArrayID);

		glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	glDisableVertexAttribArray(vertexLocation);
	glDisableVertexAttribArray(colorLocation);
	glDisableVertexAttribArray(rotationMatrixLocation);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &rotationMatrixBuffer);
	// glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

