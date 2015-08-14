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
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(640, 480, "Tutorial 03 - Matrices", NULL, NULL);
	if ( window == NULL ) {
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
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

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "vertex.glsl", "fragment.glsl" );
	// GLuint rotationMatrixID = glGetUniformLocation(programID, "rotationMatrix");

	// Get a handle for our "MVP" uniform
	// GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	// GLuint textureSamplerID = glGetUniformLocation(programID, "textureSampler");

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	// glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	// glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
	
	// Camera matrix
	// glm::mat4 View       = glm::lookAt(
	// 							glm::vec3(0,0,3), // Camera is at (4,3,3), in World Space
	// 							glm::vec3(0,0,0), // and looks at the origin
	// 							glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
	// 					   );
	// Model matrix : an identity matrix (model will be at the origin)
	// glm::mat4 Model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	// Our ModelViewProjection : multiplication of our 3 matrices
	// glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around
	// glm::mat4 MVP = Model; // Remember, matrix multiplication is the other way around
	glm::mat4 rotationMatrix1 = glm::rotate(glm::mat4(1), 0.0f, glm::vec3(0, 0, 1));
	glm::mat4 rotationMatrix2 = glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0, 0, 1));

	glm::mat4 rotationMatrices[2] = {
		rotationMatrix1,
		rotationMatrix2
	};
 
	/*
	 * non redundant vertices info
	 * +++++++++++++++++++++++++++++
	 */
	/*
	static const GLfloat quad1[] = { 
		-1.0f, -1.0f, 0.0f,	// vertex a bottom left
		 1.0f, -1.0f, 0.0f,	// vertex b bottom right
		 1.0f,  1.0f, 0.0f,	// vertex c top right
		-1.0f,  1.0f, 0.0f,	// vertex d top left
	};

	static const GLfloat quad1uv[] = { 
		0.0f, 1.0f,			// vertex a botton left
		1.0f, 1.0f,			// vertex b bottom right
		1.0f, 0.0f,			// vertex c top right
		0.0f, 0.0f,			// vertex c top left
	};

	// 3 int for each triangle
	static const GLushort quad1elements[] = {
		0, 1, 2,			// triangle abc
		0, 2, 3,			// triangle acd
	};

	static const GLfloat quad1color[] = {
		1.0f,	0.0f,	0.0f,	// vertex a bottom left
		0.0f,	1.0f,	0.0f,	// vertex b bottom left
		0.0f,	0.0f,	1.0f,	// vertex c bottom left
		1.0f,	1.0f,	0.0f,	// vertex d bottom left
	};
	*/

	/*
	 * redundant vertices info
	 * +++++++++++++++++++++++++++++
	 */
	/*
	static const GLfloat quad1[] = { 
		-1.0f, -1.0f, 0.0f,	// vertex a bottom left
		 1.0f, -1.0f, 0.0f,	// vertex b bottom right
		 1.0f,  1.0f, 0.0f,	// vertex c top right
		-1.0f, -1.0f, 0.0f,	// vertex a bottom left
		 1.0f,  1.0f, 0.0f,	// vertex c top right
		-1.0f,  1.0f, 0.0f,	// vertex d top left
	};
	static const GLfloat quad1uv[] = { 
		0.0f, 1.0f,			// vertex a botton left
		1.0f, 1.0f,			// vertex b bottom right
		1.0f, 0.0f,			// vertex c top right
		0.0f, 1.0f,			// vertex a botton left
		1.0f, 0.0f,			// vertex c top right
		0.0f, 0.0f,			// vertex d top left
	};
	static const GLfloat quad1color[] = {
		1.0f,	0.0f,	0.0f,	// vertex a bottom left
		0.0f,	1.0f,	0.0f,	// vertex b bottom left
		0.0f,	0.0f,	1.0f,	// vertex c bottom left
		1.0f,	0.0f,	0.0f,	// vertex a bottom left
		0.0f,	0.0f,	1.0f,	// vertex c bottom left
		1.0f,	1.0f,	0.0f,	// vertex d bottom left
	};
	*/

	static const GLfloat triangleVertices[] = { 
		-1.0f, -1.0f, 0.0f,	// vertex a bottom left
		 1.0f, -1.0f, 0.0f,	// vertex b bottom right
		 1.0f,  1.0f, 0.0f,	// vertex c top right
	};
	static const GLfloat triangleColors[] = {
		1.0f,	0.0f,	0.0f,	// red
		0.0f,	1.0f,	0.0f,	// green
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute index
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	/*
	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad1uv), quad1uv, GL_STATIC_DRAW);

	// 2nd attribute buffer : uv
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	*/

	GLuint colorBuffer;
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColors), triangleColors, GL_STATIC_DRAW);

	// 2nd attribute buffer : color
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glVertexAttribPointer(
		1,                  // attribute location
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glVertexAttribDivisor(1, 1);	// attrib 2 (color) will be the same for each 3 consecutive vertices

	GLuint rotationMatrixBuffer;
	glGenBuffers(1, &rotationMatrixBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, rotationMatrixBuffer);
	std::cout << "size of mat4 " << sizeof(glm::mat4) << std::endl;
	std::cout << "size of rotationMatrices: " << sizeof(rotationMatrices) << std::endl;
	glBufferData(GL_ARRAY_BUFFER, sizeof(rotationMatrices), rotationMatrices, GL_STATIC_DRAW);

	for (unsigned i = 0; i < 4; i++) {
		// set up vertex attribute
		glVertexAttribPointer(
				2+i,				// attrib index
				4,					// nr
				GL_FLOAT,
				GL_FALSE,
				sizeof(glm::mat4),
				(void *)(sizeof(glm::vec4) *i)
		);
		glEnableVertexAttribArray(2+i);
		glVertexAttribDivisor(2+1, 1);
	}

	glUseProgram(programID);
	/*
	glUniformMatrix4fv(
			rotationMatrixID,	// GLint location
			1,			// GLsizei count (1 if pointer is just 1 matrix and not an array of matrices)
			GL_FALSE,		// GLboolean transpose (specify wheter to transpose the matrix)
			&rotationMatrix2[0][0]		// GLfloat * data
	);
	*/

	/*
	 * element buffer
	 */
	/*
	GLuint elementBuffer;
	glGenBuffers(1, &elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad1elements), quad1elements, GL_STATIC_DRAW);
	*/

	/*
	GLuint texture;
	glGenTextures(1, &texture);
	// glActiveTexture(GL_TEXTURE0);		// before glBindTexture
	glBindTexture(GL_TEXTURE_2D, texture);

	int imageWidth, imageHeight;
	unsigned char *pixels = SOIL_load_image("sample.png", &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB);

	// std::cout << "image width " << imageWidth << " and height " << imageHeight << std::endl;
	glTexImage2D(
			GL_TEXTURE_2D,	// target
			0,				// level of detail. 0 = base image; n = n-th mipmap
			GL_RGB,			// how values should be stored in GPU (GPU data format)
			imageWidth, imageHeight,			// width and height
			0,				// border. MUST BE ALWAYS ZERO
			GL_RGB,			// cpu data format
			GL_UNSIGNED_BYTE,		// cpu component data type
			pixels			// cpu buffer
	);
	// check out when i have to free the memory
	SOIL_free_image_data(pixels);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// TODO: maybe I'll need to specify GL_TEXTURE_MAG_FILTER y GL_TEXTURE_MIN_FILTER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// float textureBorderColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, textureBorderColor);

	// IMPORTANT!!! ALWAYS USE A PROGRAM BEFORE CALLING glUniform
	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(
			MatrixID,		// GLint location
			1,				// GLsizei count (1 if pointer is just 1 matrix and not an array of matrices)
			GL_FALSE,		// GLboolean transpose (specify wheter to transpose the matrix)
			&MVP[0][0]		// GLfloat * data
	);

	glUniform1i(textureSamplerID, 0);	// 0 = GL_TEXTURE0
	*/

	do{

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glBindVertexArray(VertexArrayID);

		// Draw the triangle !
		// glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle
		/*
		glDrawElements(
				GL_TRIANGLES,		// primitive
				6,					// nr indices to use
				GL_UNSIGNED_SHORT,
				(void *)0
		);
		*/

		/*
		glDrawElementsInstanced(
				GL_TRIANGLES,
				6,					// nr indices to use
				GL_UNSIGNED_SHORT,
				(void *)0,
				2					// nr instances to draw
		);
		*/
		// glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorBuffer);
	// glDeleteBuffers(1, &rotationMatrixBuffer);
	// glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

