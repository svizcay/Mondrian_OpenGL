// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/controls.hpp>

#include "rectangle.hpp"
#include <vector>
#include <set>
#include <iostream>
#include <ctime>
#include <unistd.h>	// usleep()

void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
bool endSimulation = false;

std::vector<Rectangle> rectangles;

int main( void )
{

	// init random seed equal to current time
	std::srand(std::time(0));

	// nr rectangles * 2 triangles each * 3 vertices * 4 floats
	float *cpuBufferDataPoints = new float[rectangles.size() * 2 * 3 * 4];
	float *cpuBufferColors = new float[rectangles.size() * 2 * 3 * 4];

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	// window = glfwCreateWindow( 1024, 768, "Tutorial 03 - Matrices", NULL, NULL);
	window = glfwCreateWindow( 600, 600, "Mondrian", NULL, NULL);
	if( window == NULL ){
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

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	// white background
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	// VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "SimpleTransform.vertexshader", "SingleColor.fragmentshader" );

	// Use our shader
	glUseProgram(programID);

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	// glm::mat4 Projection = glm::perspective(90.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// glm::mat4 Projection = glm::perspective(90.0f, 4.0f / 4.0f, 0.1f, 100.0f);
	// glm::mat4 Projection = glm::ortho(30.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	// left, right, bottom, top, angle1, angle2
	glm::mat4 Projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f,1.0f,100.0f); // In world coordinates
	
	// Camera matrix
	glm::mat4 View       = glm::lookAt(
								glm::vec3(0,0,2), // Camera is at (4,3,3), in World Space
								glm::vec3(0,0,0), // and looks at the origin
								glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
						   );
	// glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));
	// Model matrix : an identity matrix (model will be at the origin)
	// glm::mat4 scaleM = glm::scale(glm::mat4(1.0f), glm::vec3(1,1,1));
	// glm::vec3 rotationAxis (1, 0, 0);
	// glm::mat4 rotateM = glm::rotate(glm::mat4(1.0f), 90.0f, rotationAxis);
	// glm::mat4 translateM = glm::translate(glm::mat4(1.0f), glm::vec3(1,1,1));

	// glm::mat4 Model      = glm::mat4(1.0f);
	// glm::mat4 Model      = translateM * rotateM * scaleM;
	// Our ModelViewProjection : multiplication of our 3 matrices

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	GLuint colorBuffer;
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	unsigned simulationTime = 0;

	std::set<int> verticalLines;
	std::set<int> horizontalLines;

	bool justEnded = true;
	float *cpuBufferLines;
	
	do{

		glUseProgram(programID);

		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT );

		// every 75 steps, create a new rectangle
		if (simulationTime % 75 == 0 && !endSimulation) {
			// create rectangle
			Rectangle rectangle;
			// insert rectangle into array
			rectangles.push_back(rectangle);
		}

		// update cpu buffers
		delete [] cpuBufferDataPoints;
		delete [] cpuBufferColors;
		cpuBufferDataPoints = new float[rectangles.size() * 2 * 3 * 4];
		cpuBufferColors = new float[rectangles.size() * 2 * 3 * 4];

		// fill up new cpu position buffers
		float rectangleCoords[2 * 3 * 4];
		unsigned coordCounter = 0;
		for (unsigned i = 0; i < rectangles.size(); i++) {
			rectangles[i].getCoords(rectangleCoords);
			for (unsigned j = 0; j < 2*3*4; j++) {
				cpuBufferDataPoints[coordCounter] = rectangleCoords[j];
				// std::cout << cpuBufferDataPoints[coordCounter] << std::endl;
				coordCounter++;
			}
		}

		// fill up new cpu colors buffers
		float rectangleColorComponents[2 * 3 * 4];
		coordCounter = 0;
		for (unsigned i = 0; i < rectangles.size(); i++) {
			rectangles[i].getColorComponents(rectangleColorComponents);
			for (unsigned j = 0; j < 2*3*4; j++) {
				// if (j % 4 == 0) std::cout << std::endl;
				cpuBufferColors[coordCounter] = rectangleColorComponents[j];
				// std::cout << cpuBufferColors[coordCounter] << " ";
				// std::cout << cpuBufferDataPoints[coordCounter] << std::endl;
				coordCounter++;
			}
		}

		// TODO: check if i have to enable those attrib with a bound buffer

		// transfer data to position and color buffers
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		// glBufferData(GL_ARRAY_BUFFER, sizeof(float) * rectangles.size() * 2 * 3 * 4, cpuBufferDataPoints, GL_STREAM_DRAW);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * rectangles.size() * 2 * 3 * 4, NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * rectangles.size() * 2 * 3 * 4, cpuBufferDataPoints);

		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		// glBufferData(GL_ARRAY_BUFFER, sizeof(float) * rectangles.size() * 2 * 3 * 4, cpuBufferColors, GL_STREAM_DRAW);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * rectangles.size() * 2 * 3 * 4, NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * rectangles.size() * 2 * 3 * 4, cpuBufferColors);

		for (unsigned i = 0; i < rectangles.size(); i++) {
			// std::cout << "updating rectangle id: " << i << std::endl;
			rectangles[i].updateModel();
			if (rectangles[i].shouldBeAlive()) {
				glm::mat4 Model = rectangles[i].getModel();
				glm::mat4 MVP = Projection * View * Model;
				// glm::mat4 MVP = Model;
				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
				// Draw 1 rectangle (2 triangles, 3 vertices each)
				glDrawArrays(GL_TRIANGLES, i*2*3, 2*3); // 3 indices starting at 0 -> 1 triangle
			} else {
				// std::cout << "rectangle " << i << " is dead" << std::endl;
			}
		}

		for (std::vector<Rectangle>::iterator it = rectangles.begin(); it != rectangles.end();) {
			if (it->isAlive()) {
				it++;
			} else {
				it = rectangles.erase(it);
			}
		}

		if (endSimulation) {
			if (justEnded) {
				justEnded = false;

				// get vertical and horizontal coords of every line that should be drawn
				for (unsigned i = 0; i < rectangles.size(); i++) {
					int left = rectangles[i].getLeft();
					int right = rectangles[i].getRight();
					int bottom = rectangles[i].getBottom();
					int top = rectangles[i].getTop();
					verticalLines.insert(left);
					verticalLines.insert(right);
					horizontalLines.insert(bottom);
					horizontalLines.insert(top);
				}

				unsigned nrVerticalLines = verticalLines.size();
				unsigned nrHorizontalLines = horizontalLines.size();
				unsigned nrLines = nrVerticalLines + nrHorizontalLines;
				// 1 line 2 points each; 1 point 2 coord each
				cpuBufferLines = new float[nrLines * 4];
				unsigned counter = 0;
				for (std::set<int>::iterator it = verticalLines.begin(); it != verticalLines.end(); it++) {
					cpuBufferLines[counter++] = *it;
					cpuBufferLines[counter++] = 10;
					cpuBufferLines[counter++] = *it;
					cpuBufferLines[counter++] = -10;
				}

				for (std::set<int>::iterator it = horizontalLines.begin(); it != horizontalLines.end(); it++) {
					cpuBufferLines[counter++] = -10;
					cpuBufferLines[counter++] = *it;
					cpuBufferLines[counter++] = 10;
					cpuBufferLines[counter++] = *it;
				}
			}

			// glUseProgram(programID2);
		}

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		// usleep(500000);
		// usleep(100000);

		simulationTime++;

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );


	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	delete [] cpuBufferDataPoints;
	delete [] cpuBufferColors;
	delete [] cpuBufferLines;

	return 0;
}

void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
	int windowWidth;
	int windowHeight;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		// std::cout << "click on: " << xpos << " " << ypos << std::endl;
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		// std::cout << "window's size: " << windowWidth << " " << windowHeight << std::endl;
		double normalizedX = xpos * 1.0 / windowWidth;
		double normalizedY = ypos * 1.0 / windowHeight;
		double modelCoordX;
		double modelCoordY;
		// std::cout << "click on (normalized): " << normalizedX << " " << normalizedY << std::endl;
		// TODO: change hardcoded value 10
		if (normalizedX > 0.5) {
			// positive X
			modelCoordX = normalizedX * 20 - 10;
		} else {
			// negative X
			modelCoordX = normalizedX * 20 - 10;
		}

		if (normalizedY > 0.5) {
			// negative Y
			modelCoordY = normalizedY * 20;
			modelCoordY = -(modelCoordY - 10);
		} else {
			// positive Y
			modelCoordY = normalizedY * 20;
			modelCoordY = (10 - modelCoordY);
		}

		// std::cout << "(" << modelCoordX << "," << modelCoordY << ")" << std::endl;
		// usleep(3000000);

		for (unsigned i = 0; i < rectangles.size(); i++) {
			rectangles[i].checkPinned(modelCoordX, modelCoordY);
		}
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		endSimulation = true;
		std::cout << "right click" << std::endl;
	}
}
