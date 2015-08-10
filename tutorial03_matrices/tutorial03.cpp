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
#include <cmath>

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
		std::cerr << "Failed to open GLFW window.";
		std::cerr << " If you have an Intel GPU, they are not 3.3 compatible.";
		std::cerr << " Try the 2.1 version of the tutorials." << std::endl;
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

	// enable z-buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

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
	float cpuBufferMVProw1[MAX_NR_RECTANGLES * 2 * 3 * 4];
	float cpuBufferMVProw2[MAX_NR_RECTANGLES * 2 * 3 * 4];
	float cpuBufferMVProw3[MAX_NR_RECTANGLES * 2 * 3 * 4];
	float cpuBufferMVProw4[MAX_NR_RECTANGLES * 2 * 3 * 4];
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
	/*
	glm::mat4 Projection = glm::perspective(
		45.0f,         // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
		4.0f / 4.0f, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
		0.1f,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
		100.0f       // Far clipping plane. Keep as little as possible.
	);		
	*/
	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(0,0,20), // Camera is at (4,3,3), in World Space
		glm::vec3(0,0,0), // and looks at the origin
		glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	GLuint rectangleVertexBuffer;
	glGenBuffers(1, &rectangleVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, rectangleVertexBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,					// attribute. No particular reason for 0, but must match the layout in the shader.
		4,					// size
		GL_FLOAT,			// type
		GL_FALSE,			// normalized?
		0,					// stride
		(void*)0			// array buffer offset
	);

	GLuint rectangleColorBuffer;
	glGenBuffers(1, &rectangleColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, rectangleColorBuffer);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,					// attribute. No particular reason for 0, but must match the layout in the shader.
		4,					// size
		GL_FLOAT,			// type
		GL_FALSE,			// normalized?
		0,					// stride
		(void*)0			// array buffer offset
	);

	// GLuint rectangleMVPBuffer;
	GLuint rectangleMVProw1Buffer;
	GLuint rectangleMVProw2Buffer;
	GLuint rectangleMVProw3Buffer;
	GLuint rectangleMVProw4Buffer;
	glGenBuffers(1, &rectangleMVProw1Buffer);
	glGenBuffers(1, &rectangleMVProw2Buffer);
	glGenBuffers(1, &rectangleMVProw3Buffer);
	glGenBuffers(1, &rectangleMVProw4Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, rectangleMVProw1Buffer);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,										// attribute. No particular reason for 0, but must match the layout in the shader.
		4,												// size
		GL_FLOAT,								// type
		GL_FALSE,								// normalized?
		0,		// stride
		(void*)(0)								// array buffer offset
	);
	glBindBuffer(GL_ARRAY_BUFFER, rectangleMVProw2Buffer);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(
		3,										// attribute. No particular reason for 0, but must match the layout in the shader.
		4,												// size
		GL_FLOAT,								// type
		GL_FALSE,								// normalized?
		0,		// stride
		(void*)(0)								// array buffer offset
	);
	glBindBuffer(GL_ARRAY_BUFFER, rectangleMVProw3Buffer);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(
		4,										// attribute. No particular reason for 0, but must match the layout in the shader.
		4,												// size
		GL_FLOAT,								// type
		GL_FALSE,								// normalized?
		0,		// stride
		(void*)(0)								// array buffer offset
	);
	glBindBuffer(GL_ARRAY_BUFFER, rectangleMVProw4Buffer);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(
		5,										// attribute. No particular reason for 0, but must match the layout in the shader.
		4,												// size
		GL_FLOAT,								// type
		GL_FALSE,								// normalized?
		0,		// stride
		(void*)(0)								// array buffer offset
	);

	glBindVertexArray(lineVAO);
	GLuint lineVertexBuffer;
	glGenBuffers(1, &lineVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, lineVertexBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,					// attribute. No particular reason for 0, but must match the layout in the shader.
		2,					// size
		GL_FLOAT,			// type
		GL_FALSE,			// normalized?
		0,					// stride
		(void*)0			// array buffer offset
	);

	// get a handle for MVP uniform matrix in line program (it's not required
	// to be using the line program right now
	// GLuint lineMVPID = glGetUniformLocation(lineProgram, "MVP");

	glBindVertexArray(rectangleVAO);


	std::set<float> verticalLines;
	std::set<float> horizontalLines;
	unsigned nrLines = 0;

	unsigned simulationTime = 0;
	bool justEnded = true;

	double lineThickness = 0.1;
		
	do {

		// update window's title to show fps
		updateFPSCounter(window);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// update viewport 
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		glViewport(0, 0, windowWidth, windowHeight);	// (x,y) offset from lower left; (width, height)

		// std::cout << "nr rectangles: " << rectangles.size() << std::endl;

		// every 75 steps, create a new rectangle
		if (simulationTime % 500 == 0 && !endSimulation && rectangles.size() < MAX_NR_RECTANGLES) {
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
				// std::cout << "rectangle ID: " << rectangles[i].getID() << std::endl;
				// std::cout << "horizontal: [" << rectangles[i].getLeft() << " : " << rectangles[i].getRight() << "]" << std::endl;
				// std::cout << "vertical: [" << rectangles[i].getBottom() << " : " << rectangles[i].getTop() << "]" << std::endl;
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
		unsigned counter = 0;
		for (unsigned i = 0; i < rectangles.size(); i++) {
			glm::mat4 Model = rectangles[i].getModel();
			glm::mat4 MVP = Projection * View * Model;
			// TODO: try to transfer glm::mat MVP to buffer directly with glBufferData
			glm::vec4 vertexa = Model * rectangles[i].getVertexA();
			glm::vec4 vertexb = Model * rectangles[i].getVertexB();
			glm::vec4 vertexc = Model * rectangles[i].getVertexC();
			glm::vec4 vertexd = Model * rectangles[i].getVertexD();

			// std::cout << "final positions: " << std::endl;
			// std::cout << vertexa.x << " " << vertexa.y << " " << vertexa.z << std::endl;
			// std::cout << vertexb.x << " " << vertexb.y << " " << vertexb.z << std::endl;
			// std::cout << vertexc.x << " " << vertexc.y << " " << vertexc.z << std::endl;
			// std::cout << vertexd.x << " " << vertexd.y << " " << vertexd.z << std::endl;

			for (unsigned vertex = 0; vertex < 6; vertex++) {
				for (unsigned element = 0; element < 4; element++) {
					// cpuBufferMVProw1[counter] = static_cast<int>(MVP[0][element]);
					// cpuBufferMVProw2[counter] = static_cast<int>(MVP[1][element]);
					// cpuBufferMVProw3[counter] = static_cast<int>(MVP[2][element]);
					// cpuBufferMVProw4[counter] = static_cast<int>(MVP[3][element]);
					cpuBufferMVProw1[counter] = MVP[0][element];
					cpuBufferMVProw2[counter] = MVP[1][element];
					cpuBufferMVProw3[counter] = MVP[2][element];
					cpuBufferMVProw4[counter] = MVP[3][element];
					// std::cout << "double: " << std::endl;
					// std::cout << MVP[0][element] << " ";
					// std::cout << MVP[1][element] << " ";
					// std::cout << MVP[2][element] << " ";
					// std::cout << MVP[3][element] << std::endl;
					// std::cout << "int: " << std::endl;
					// std::cout << cpuBufferMVProw1[counter] << " ";
					// std::cout << cpuBufferMVProw2[counter] << " ";
					// std::cout << cpuBufferMVProw3[counter] << " ";
					// std::cout << cpuBufferMVProw4[counter] << std::endl;
					counter++;
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

		glBindBuffer(GL_ARRAY_BUFFER, rectangleMVProw1Buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * rectangles.size() * 2 * 3 * 4, cpuBufferMVProw1, GL_STREAM_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, rectangleMVProw2Buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * rectangles.size() * 2 * 3 * 4, cpuBufferMVProw2, GL_STREAM_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, rectangleMVProw3Buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * rectangles.size() * 2 * 3 * 4, cpuBufferMVProw3, GL_STREAM_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, rectangleMVProw4Buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * rectangles.size() * 2 * 3 * 4, cpuBufferMVProw4, GL_STREAM_DRAW);

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
			glDrawArrays(GL_TRIANGLES, 0, nrLines * 2 * 3); // 3 indices starting at 0 -> 1 triangle
		}

		if (endSimulation && justEnded) {

			std::cout << "creating lines..." << std::endl;
			justEnded = false;

			// load data in cpuBufferLines just the very first time

			// get vertical and horizontal coords of every line that should be drawn
			for (unsigned i = 0; i < rectangles.size(); i++) {
				if (rectangles[i].getIsPinned()) {
					// values in {-10:10}
					int left = static_cast<int>(round(rectangles[i].getLeft()));
					int right = static_cast<int>(round(rectangles[i].getRight()));
					int bottom = static_cast<int>(round(rectangles[i].getBottom()));
					int top = static_cast<int>(round(rectangles[i].getTop()));
					std::cout << "left: " << left << " right: " << right << " top: " << top << " bottom: " << bottom << std::endl;
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
			// 1 line = 2 triangles; 1 triangle = 3 points each; 1 point = 2 coord each;
			cpuBufferLines = new float[nrLines * 2 * 3 * 2];
			unsigned counter = 0;
			glm::mat4 PV = Projection * View;
			for (std::set<float>::iterator it = verticalLines.begin(); it != verticalLines.end(); it++) {
				glm::vec4 vertexA (*it - lineThickness, 10, 0, 1);
				glm::vec4 vertexB (*it + lineThickness, 10, 0, 1);
				glm::vec4 vertexC (*it - lineThickness, -10, 0, 1);
				glm::vec4 vertexD (*it + lineThickness, -10, 0, 1);
				vertexA = PV * vertexA;
				vertexB = PV * vertexB;
				vertexC = PV * vertexC;
				vertexD = PV * vertexD;
				// glm::vec4 firstPointMVP = Projection * View * firstPoint;
				// glm::vec4 secondPointMVP = Projection * View * secondPoint;
				// sent the points in {-10:10} domain
				// first triangle ABC
				cpuBufferLines[counter] = vertexA.x;		// x
				counter++;
				cpuBufferLines[counter] = vertexA.y;		// y
				counter++;
				cpuBufferLines[counter] = vertexB.x;		// x
				counter++;
				cpuBufferLines[counter] = vertexB.y;		// y
				counter++;
				cpuBufferLines[counter] = vertexC.x;		// x
				counter++;
				cpuBufferLines[counter] = vertexC.y;		// y
				counter++;
				// second triangle BCD
				cpuBufferLines[counter] = vertexB.x;		// x
				counter++;
				cpuBufferLines[counter] = vertexB.y;		// y
				counter++;
				cpuBufferLines[counter] = vertexC.x;		// x
				counter++;
				cpuBufferLines[counter] = vertexC.y;		// y
				counter++;
				cpuBufferLines[counter] = vertexD.x;		// x
				counter++;
				cpuBufferLines[counter] = vertexD.y;		// y
				counter++;
				// std::cout << "vertical line: " << std::endl;
				// std::cout << "(" << firstPointMVP.x << "," << firstPointMVP.y << ")" << std::endl;
				// std::cout << "(" << secondPointMVP.x << "," << secondPointMVP.y << ")" << std::endl;
			}

			for (std::set<float>::iterator it = horizontalLines.begin(); it != horizontalLines.end(); it++) {
				glm::vec4 vertexA (-10, *it + lineThickness, 0, 1);
				glm::vec4 vertexB (10, *it + lineThickness, 0, 1);
				glm::vec4 vertexC (-10, *it - lineThickness, 0, 1);
				glm::vec4 vertexD (10, *it - lineThickness, 0, 1);
				vertexA = PV * vertexA;
				vertexB = PV * vertexB;
				vertexC = PV * vertexC;
				vertexD = PV * vertexD;
				// glm::vec4 firstPointMVP = Projection * View * firstPoint;
				// glm::vec4 secondPointMVP = Projection * View * secondPoint;
				// first triangle ABC
				cpuBufferLines[counter] = vertexA.x;		// x
				counter++;
				cpuBufferLines[counter] = vertexA.y;		// y
				counter++;
				cpuBufferLines[counter] = vertexB.x;		// x
				counter++;
				cpuBufferLines[counter] = vertexB.y;		// y
				counter++;
				cpuBufferLines[counter] = vertexC.x;		// x
				counter++;
				cpuBufferLines[counter] = vertexC.y;		// y
				counter++;
				// second triangle BCD
				cpuBufferLines[counter] = vertexB.x;		// x
				counter++;
				cpuBufferLines[counter] = vertexB.y;		// y
				counter++;
				cpuBufferLines[counter] = vertexC.x;		// x
				counter++;
				cpuBufferLines[counter] = vertexC.y;		// y
				counter++;
				cpuBufferLines[counter] = vertexD.x;		// x
				counter++;
				cpuBufferLines[counter] = vertexD.y;		// y
				counter++;
				// std::cout << "horizontal line: " << std::endl;
				// std::cout << "(" << firstPointMVP.x << "," << firstPointMVP.y << ")" << std::endl;
				// std::cout << "(" << secondPointMVP.x << "," << secondPointMVP.y << ")" << std::endl;
			}

			glBindBuffer(GL_ARRAY_BUFFER, lineVertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nrLines * 2 * 3 * 2, cpuBufferLines, GL_STATIC_DRAW);


			// send MVP matrix to line program
			// TODO: check if i need to be using line program and to have bound line VAO
			// glUniformMatrix4fv(lineMVPID, 1, GL_FALSE, &PV[0][0]);
		}

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		// usleep(500000);
		// usleep(100000);

		simulationTime++;

		// int dummy;
		// std::cin >> dummy;

	} while ( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );


	glBindVertexArray(rectangleVAO);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glBindVertexArray(lineVAO);
	glDisableVertexAttribArray(0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &rectangleVertexBuffer);
	glDeleteBuffers(1, &rectangleColorBuffer);
	glDeleteBuffers(1, &rectangleMVProw1Buffer);
	glDeleteBuffers(1, &rectangleMVProw2Buffer);
	glDeleteBuffers(1, &rectangleMVProw3Buffer);
	glDeleteBuffers(1, &rectangleMVProw4Buffer);
	glDeleteBuffers(1, &lineVertexBuffer);
	glDeleteProgram(rectangleProgram);
	glDeleteProgram(lineProgram);
	glDeleteVertexArrays(1, &rectangleVAO);
	glDeleteVertexArrays(1, &lineVAO);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	delete [] cpuBufferLines;				// make sure new was executed, i.e: right click event

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
