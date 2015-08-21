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

#include <iostream>
#include <ctime>		// std::time
#include <chrono>
#include <iomanip>		// std::setw
#include <sstream>
#include <string>
#include <unistd.h>		// usleep()

#include "rectangles.hpp"
#include <vector>


void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
void updateFPSCounter(GLFWwindow * window);

GLFWwindow* window;
bool endSimulation = false;

const int worldWidth = 20;
const int worldHeight = 20;

const unsigned MAX_NR_RECTANGLES = 100;
const unsigned MAX_SIZE = 10;
const double LINE_THICKNESS = 0.1;
const int INVALID_POSITION = 100;

Rectangles rectangles (worldWidth, worldHeight,
		MAX_NR_RECTANGLES, MAX_SIZE, INVALID_POSITION,
		LINE_THICKNESS
);

int main( void )
{
	// init random seed equal to current time
	std::srand(std::time(0));

	const GLfloat basicQuad[] = {
		-1.0f,	 1.0f,		// top left
		 1.0f,	 1.0f,		// top right
		-1.0f,	-1.0f,		// bottom left
		 1.0f,	-1.0f,		// bottom right
	};

	// MAX_NR_RECTANGLES positions (x,y) in world space
	GLfloat positions[MAX_NR_RECTANGLES * 2];
	// MAX_NR_RECTANGLES scales (x,y) in subdomain world space
	GLfloat sizes[MAX_NR_RECTANGLES * 2];
	// MAX_NR_RECTANGLES colors (r,g,b)
	GLfloat colors[MAX_NR_RECTANGLES * 3];


	// windows size
	int windowWidth = 600;
	int windowHeight = 600;

	// Initialise GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(windowWidth, windowHeight, "Mondrian", NULL, NULL);
	if (window == NULL) {
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

	// Create and compile our GLSL program from the shaders
	GLuint shaderProgram = LoadShaders("vertex.glsl", "fragment.glsl");

	// Use our shader (it's not required to be using a program to bind VAOs)
	glUseProgram(shaderProgram);

	// VAOs
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	GLint positionLoc = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(positionLoc);
	glVertexAttribPointer(
		positionLoc,		// attribute.
		2,					// size
		GL_FLOAT,			// type
		GL_FALSE,			// normalized?
		0,					// stride
		(void*)0			// array buffer offset
	);
	glBufferData(GL_ARRAY_BUFFER, sizeof(basicQuad), basicQuad, GL_STATIC_DRAW);

	GLuint windowSizeLoc	= glGetUniformLocation(shaderProgram, "windowSize");
	GLuint positionsLoc		= glGetUniformLocation(shaderProgram, "positions");
	GLuint sizesLoc			= glGetUniformLocation(shaderProgram, "sizes");
	GLuint colorsLoc		= glGetUniformLocation(shaderProgram, "colors");

	unsigned simulationTime = 0;
	bool justEnded = true;

	do {

		// update window's title to show fps
		updateFPSCounter(window);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// update viewport
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		glViewport(0, 0, windowWidth, windowHeight);	// (x,y) offset from lower left; (width, height)

		// every 75 steps, create a new rectangle
		if (simulationTime % 100 == 0 && !endSimulation) {
			rectangles.createOne();
		}

		// update positions
		rectangles.update();

		// update cpu buffers
		rectangles.getPositions(positions);
		rectangles.getSizes(sizes);
		rectangles.getColors(colors);

		// transfer data to uniforms
		glUniform2f(windowSizeLoc, windowWidth, windowHeight);
		glUniform2fv(positionsLoc, MAX_NR_RECTANGLES, positions);
		glUniform2fv(sizesLoc, MAX_NR_RECTANGLES, sizes);
		glUniform3fv(colorsLoc, MAX_NR_RECTANGLES, colors);

		// draw rectangles
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // 3 indices starting at 0 -> 1 triangle

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		simulationTime++;
		// usleep(500000);

	} while ( glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
			glfwWindowShouldClose(window) == 0 );

	// Cleanup vao, vbo, and program
	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1, &vao);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

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
		// std::cout << "ndc (" << ndcx << "," << ndcy << ")" << std::endl;

		// world coordinates
		double worldx = ndcx * worldWidth / 2;
		double worldy = ndcy * worldHeight / 2;

		// std::cout << "world (" << worldx << "," << worldy << ")" << std::endl;
		// usleep(3000000);



		/*
		for (unsigned i = 0; i < rectangles.size(); i++) {
			rectangles[i].checkPinned(worldx, worldy);
		}
		*/
		rectangles.checkPinned(worldx, worldy);
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		endSimulation = true;
		rectangles.finish();
		std::cout << "right click" << std::endl;
	}
}

void updateFPSCounter(GLFWwindow * window)
{
	static int frameCount = 0;
	static double previousTime = glfwGetTime();
	double currentTime = glfwGetTime();
	double elapsedTime = currentTime - previousTime;

	// take averages every 0.25 seconds
	if (elapsedTime > 0.25) {
		previousTime = currentTime;
		double fps = static_cast<double>(frameCount) / elapsedTime;
		std::stringstream ss;
		ss << "Mondrian @ fps(" << fps << ")";
		glfwSetWindowTitle(window, ss.str().c_str());
		frameCount = 0;
	}

	frameCount++;
}
