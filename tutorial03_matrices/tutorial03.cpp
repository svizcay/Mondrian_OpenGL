// Include standard headers
#include <cstdlib>

// Include GLEW (always before glfw)
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common/shader.hpp>
#include <common/controls.hpp>

#include "rectangle.hpp"
#include <vector>
#include <set>
#include <iostream>
#include <ctime>
// #include <unistd.h>	// usleep()
#include <sstream>
#include <string>

void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
void updateFPSCounter(GLFWwindow * window);

GLFWwindow* window;
std::vector<Rectangle> rectangles;
bool endSimulation = false;

int main( void )
{

	// init random seed equal to current time
	std::srand(std::time(0));

	const unsigned MAX_NR_RECTANGLES = 100;

	// windows size
	int windowWidth = 600;
	int windowHeight = 600;

	// Initialise GLFW
	if(!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	// window = glfwCreateWindow( 1024, 768, "Tutorial 03 - Matrices", NULL, NULL);
	window = glfwCreateWindow(windowWidth, windowHeight, "Mondrian", NULL, NULL);
	if(window == NULL) {
		std::cerr << "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	// white background
	glm::vec4 whiteColor (1.0f, 1.0f, 1.0f, 0.0f);
	glClearColor(whiteColor.r, whiteColor.g, whiteColor.b, whiteColor.a);

	// nr rectangles * 2 triangles each * 3 vertices * 4 floats
	float cpuBufferDataPoints[MAX_NR_RECTANGLES * 2 * 3 * 4];
	float cpuBufferColors[MAX_NR_RECTANGLES * 2 * 3 * 4];
	// nr rectangles * 2 triangles each * 3 vertices * 4x4 floats
	float cpuBufferMVP[MAX_NR_RECTANGLES * 2 * 3 * 4 * 4];
	float *cpuBufferLines;

	// Create and compile our GLSL program from the shaders
	GLuint rectangleProgram = LoadShaders( "rectangleVertexShader.glsl", "rectangleFragmentShader.glsl" );
	GLuint lineProgram = LoadShaders( "lineVertexShader.glsl", "lineFragmentShader.glsl" );

	// Use our shader (it's not required to be using a program to bind VAOs)
	glUseProgram(rectangleProgram);

	// VAOs
	GLuint rectangleVAO;
	GLuint lineVAO;
	glGenVertexArrays(1, &rectangleVAO);
	glGenVertexArrays(1, &lineVAO);
	glBindVertexArray(rectangleVAO);

	// left, right, bottom, top, angle1, angle2
	glm::mat4 Projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f,1.0f,100.0f); // In world coordinates
	// glm::mat4 Projection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f,1.0f,100.0f); // In world coordinates
	
	// Camera matrix
	glm::mat4 View = glm::lookAt(
								glm::vec3(0,0,2), // Camera is at (4,3,3), in World Space
								glm::vec3(0,0,0), // and looks at the origin
								glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	GLuint rectangleVertexBuffer;
	glGenBuffers(1, &rectangleVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, rectangleVertexBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	GLuint rectangleColorBuffer;
	glGenBuffers(1, &rectangleColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, rectangleColorBuffer);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	GLuint rectangleMVPBuffer;
	glGenBuffers(1, &rectangleMVPBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, rectangleMVPBuffer);
	int pos1 = 2+0;
	int pos2 = 2+1;
	int pos3 = 2+2;
	int pos4 = 2+3;
	glEnableVertexAttribArray(pos1);
	glEnableVertexAttribArray(pos2);
	glEnableVertexAttribArray(pos3);
	glEnableVertexAttribArray(pos4);
	glVertexAttribPointer(
		pos1,					// attribute. No particular reason for 0, but must match the layout in the shader.
		4,						// size
		GL_FLOAT,				// type
		GL_FALSE,				// normalized?
		sizeof(float) * 4 * 4,	// stride
		(void*)(0)				// array buffer offset
	);
	glVertexAttribPointer(
		pos2,					// attribute. No particular reason for 0, but must match the layout in the shader.
		4,						// size
		GL_FLOAT,				// type
		GL_FALSE,				// normalized?
		sizeof(float) * 4 * 4,	// stride
		(void*)(sizeof(float) * 4)				// array buffer offset
	);
	glVertexAttribPointer(
		pos3,					// attribute. No particular reason for 0, but must match the layout in the shader.
		4,						// size
		GL_FLOAT,				// type
		GL_FALSE,				// normalized?
		sizeof(float) * 4 * 4,	// stride
		(void*)(sizeof(float) * 8)				// array buffer offset
	);
	glVertexAttribPointer(
		pos4,					// attribute. No particular reason for 0, but must match the layout in the shader.
		4,						// size
		GL_FLOAT,				// type
		GL_FALSE,				// normalized?
		sizeof(float) * 4 * 4,	// stride
		(void*)(sizeof(float) * 12)				// array buffer offset
	);
	glVertexAttribDivisor(pos1, 1);
	glVertexAttribDivisor(pos2, 1);
	glVertexAttribDivisor(pos3, 1);
	glVertexAttribDivisor(pos4, 1);

	glBindVertexArray(lineVAO);
	GLuint lineVertexBuffer;
	glGenBuffers(1, &lineVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, lineVertexBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glBindVertexArray(rectangleVAO);


	std::set<float> verticalLines;
	std::set<float> horizontalLines;
	unsigned nrLines = 0;

	unsigned simulationTime = 0;
	bool justEnded = true;
	
	do {

		// update window's title to show fps
		updateFPSCounter(window);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// update viewport 
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		glViewport(0, 0, windowWidth, windowHeight);	// (x,y) offset from lower left; (width, height)

		// std::cout << "nr rectangles: " << rectangles.size() << std::endl;


		// every 75 steps, create a new rectangle
		if (simulationTime % 75 == 0 && !endSimulation && rectangles.size() < MAX_NR_RECTANGLES) {
			// create rectangle
			Rectangle rectangle;
			// insert rectangle into array
			rectangles.push_back(rectangle);
		}

		// update cpu buffers

		// fill up new cpu position buffers
		float rectangleCoords[2 * 3 * 4];
		unsigned coordCounter = 0;
		for (unsigned i = 0; i < rectangles.size(); i++) {
			rectangles[i].getCoords(rectangleCoords);
			for (unsigned j = 0; j < 2*3*4; j++) {
				cpuBufferDataPoints[coordCounter] = rectangleCoords[j];
				coordCounter++;
			}
		}

		// fill up new cpu colors buffers
		float rectangleColorComponents[2 * 3 * 4];
		coordCounter = 0;
		for (unsigned i = 0; i < rectangles.size(); i++) {
			rectangles[i].getColorComponents(rectangleColorComponents);
			for (unsigned j = 0; j < 2*3*4; j++) {
				cpuBufferColors[coordCounter] = rectangleColorComponents[j];
				coordCounter++;
			}
		}

		// fill up new cpu mvp buffers
		for (unsigned i = 0; i < rectangles.size(); i++) {
			glm::mat4 Model = rectangles[i].getModel();
			glm::mat4 MVP = Projection * View * Model;
			// TODO: try to transfer glm::mat MVP to buffer directly with glBufferData
			unsigned counter = 0;

			// for (unsigned row = 0; row < 4; row++) {
			// 	for (unsigned col = 0; col < 4; col++) {
			// 		for (unsigned vertex = 0; vertex < 6; vertex++) {
			// 			// TODO: verify this array index
			// 			cpuBufferMVP[i * vertex * 6 + counter] = MVP[row][col];
			// 		}
			// 		counter ++;
			// 	}
			// }

			for (unsigned vertex = 0; vertex < 6; vertex++) {
				for (unsigned row = 0; row < 4; row++) {
					for (unsigned col = 0; col < 4; col++) {
						cpuBufferMVP[counter] = MVP[row][col];
						counter++;
					}
				}
			}
		}

		glUseProgram(rectangleProgram);
		glBindVertexArray(rectangleVAO);

		// transfer data to position and color buffers
		glBindBuffer(GL_ARRAY_BUFFER, rectangleVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * rectangles.size() * 2 * 3 * 4, cpuBufferDataPoints, GL_STREAM_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, rectangleColorBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * rectangles.size() * 2 * 3 * 4, cpuBufferColors, GL_STREAM_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, rectangleMVPBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * rectangles.size() * 2 * 3 * 4 * 4, cpuBufferMVP, GL_STREAM_DRAW);

		// draw rectangles
		glDrawArrays(GL_TRIANGLES, 0, rectangles.size()*2*3); // 3 indices starting at 0 -> 1 triangle

		for (unsigned i = 0; i < rectangles.size(); i++) {
			// std::cout << "updating rectangle id: " << i << std::endl;
			rectangles[i].updateModel();
			rectangles[i].shouldBeAlive();
		}

		for (std::vector<Rectangle>::iterator it = rectangles.begin(); it != rectangles.end();) {
			if (it->isAlive()) {
				it++;
			} else {
				it = rectangles.erase(it);
			}
		}

		if (endSimulation && !justEnded) {
			glUseProgram(lineProgram);
			glBindVertexArray(lineVAO);
			// std::cout << "drawing lines..." << std::endl;
			glDrawArrays(GL_LINES, 0, nrLines * 2); // 3 indices starting at 0 -> 1 triangle
		}

		if (endSimulation && justEnded) {

			std::cout << "creating lines..." << std::endl;
			justEnded = false;

			// load data in cpuBufferLines just the very first time

			// get vertical and horizontal coords of every line that should be drawn
			for (unsigned i = 0; i < rectangles.size(); i++) {
				if (rectangles[i].getIsPinned()) {
					float left = rectangles[i].getLeft();
					float right = rectangles[i].getRight();
					float bottom = rectangles[i].getBottom();
					float top = rectangles[i].getTop();
					// std::cout << "left: " << left << " right: " << right << " top: " << top << " bottom: " << bottom << std::endl;
					verticalLines.insert(left);
					verticalLines.insert(right);
					horizontalLines.insert(bottom);
					horizontalLines.insert(top);
				}
			}

			unsigned nrVerticalLines = verticalLines.size();
			unsigned nrHorizontalLines = horizontalLines.size();
			nrLines = nrVerticalLines + nrHorizontalLines;
			std::cout << "nr lines to draw: " << nrLines << std::endl;
			// 1 line = 2 points each; 1 point = 2 coord each;
			cpuBufferLines = new float[nrLines * 2 * 2];
			unsigned counter = 0;
			for (std::set<float>::iterator it = verticalLines.begin(); it != verticalLines.end(); it++) {
				glm::vec4 firstPoint (*it, 10, 0, 1);
				glm::vec4 secondPoint (*it, -10, 0, 1);
				glm::vec4 firstPointMVP = Projection * View * firstPoint;
				glm::vec4 secondPointMVP = Projection * View * secondPoint;
				// first point
				cpuBufferLines[counter] = firstPointMVP.x;	// x
				counter++;
				cpuBufferLines[counter] = firstPointMVP.y;		// y
				counter++;
				// second point
				cpuBufferLines[counter] = secondPointMVP.x;	// x
				counter++;
				cpuBufferLines[counter] = secondPointMVP.y;	// y
				counter++;
				// std::cout << "vertical line: " << std::endl;
				// std::cout << "(" << firstPointMVP.x << "," << firstPointMVP.y << ")" << std::endl;
				// std::cout << "(" << secondPointMVP.x << "," << secondPointMVP.y << ")" << std::endl;
			}

			for (std::set<float>::iterator it = horizontalLines.begin(); it != horizontalLines.end(); it++) {
				glm::vec4 firstPoint (-10, *it, 0, 1);
				glm::vec4 secondPoint (10, *it, 0, 1);
				glm::vec4 firstPointMVP = Projection * View * firstPoint;
				glm::vec4 secondPointMVP = Projection * View * secondPoint;
				// first point
				cpuBufferLines[counter] = firstPointMVP.x;	// x
				counter++;
				cpuBufferLines[counter] = firstPointMVP.y;	// y
				counter++;
				// second point
				cpuBufferLines[counter] = secondPointMVP.x;		// x
				counter++;
				cpuBufferLines[counter] = secondPointMVP.y;	// y
				counter++;
				// std::cout << "horizontal line: " << std::endl;
				// std::cout << "(" << firstPointMVP.x << "," << firstPointMVP.y << ")" << std::endl;
				// std::cout << "(" << secondPointMVP.x << "," << secondPointMVP.y << ")" << std::endl;
			}

			glBindBuffer(GL_ARRAY_BUFFER, lineVertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nrLines * 2 * 2, cpuBufferLines, GL_STATIC_DRAW);
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


	glBindVertexArray(rectangleVAO);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glBindVertexArray(lineVAO);
	glDisableVertexAttribArray(0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &rectangleVertexBuffer);
	glDeleteBuffers(1, &rectangleColorBuffer);
	glDeleteBuffers(1, &rectangleMVPBuffer);
	glDeleteBuffers(1, &lineVertexBuffer);
	glDeleteProgram(rectangleProgram);
	glDeleteProgram(lineProgram);
	glDeleteVertexArrays(1, &rectangleVAO);
	glDeleteVertexArrays(1, &lineVAO);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	delete [] cpuBufferLines;		// make sure new was executed, i.e: right click event

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

		// normalized device coordinates
		double ndcx = 2.0 * xpos / windowWidth - 1.0;
		double ndcy = 1.0 - (2.0 * ypos) / windowHeight;

		// std::cout << "(" << ndcx << "," << ndcy << ")" << std::endl;

		// world coordinates
		double worldx = ndcx * 10;
		double worldy = ndcy * 10;

		// std::cout << "(" << worldx << "," << worldy << ")" << std::endl;
		// usleep(3000000);

		for (unsigned i = 0; i < rectangles.size(); i++) {
			rectangles[i].checkPinned(worldx, worldy);
		}
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		endSimulation = true;
		std::cout << "right click" << std::endl;
	}
}

void updateFPSCounter(GLFWwindow * window)
{
	static double previousTime = glfwGetTime();
	static int frameCount = 0;
	double currentTime = glfwGetTime();
	double elapsedTime = currentTime - previousTime;

	// take averages every 0.25 seconds
	if (elapsedTime > 0.25) {
		previousTime = currentTime;
		double fps = static_cast<double>(frameCount) / elapsedTime;
		std::stringstream ss;
		ss << "Mondrian @fps(" << fps << ")";
		glfwSetWindowTitle(window, ss.str().c_str());
		frameCount = 0;
	}

	frameCount++;
}
