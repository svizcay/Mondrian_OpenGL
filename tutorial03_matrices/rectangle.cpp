#include "rectangle.hpp"
#include <iostream>
#include <glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>
#include <ctime>
#include <cmath>

const float Rectangle::surfaceSize = 1;
unsigned Rectangle::counter = 0;
// double Rectangle::previousTime = 0;
// double Rectangle::currentTime = 0;

Rectangle::Rectangle()
{
	id = counter;
	proportion = getRandomProportion();	// set it random
	color = getRandomColor();			// set it random among fixed values

	position = glm::vec4(0, 0, 0, 1);

	scaleMatrix = glm::scale(glm::mat4(1), glm::vec3(2, 2, 1));
	rotationMatrix = glm::rotate(glm::mat4(1), 0.0f, glm::vec3(0, 0, 1));
	translationMatrix = getInitialPosition();
	// modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
	// TODO change next lines
	modelMatrix = translationMatrix * rotationMatrix * scaleMatrix * glm::mat4(1);
	// modelMatrix = translationMatrix * glm::mat4(1);

	isPinned = false;
	isDead = false;

	previousTime = glfwGetTime();
	currentTime = glfwGetTime();

	counter++;

	// rectangle's vertices (object space) [-1:1]
	float ratio = proportion.x / proportion.y;
	float scaleFactor = 0;
	if (ratio > 1) {
		scaleFactor = 2 / proportion.x;
	} else {
		scaleFactor = 2 / proportion.y;
	}
	float halfx = proportion.x / 2.0;
	float halfy = proportion.y / 2.0;
	float scaledHalfx = scaleFactor * halfx;
	float scaledHalfy = scaleFactor * halfy;

	// int zbuffer = static_cast<int>(std::rand() * 1.0 / RAND_MAX * 2 - 1);
	zbuffer = 0.0;

	vertexa = glm::vec4(-scaledHalfx, scaledHalfy, zbuffer, 1);
	vertexb = glm::vec4(scaledHalfx, scaledHalfy, zbuffer, 1);
	vertexc = glm::vec4(-scaledHalfx, -scaledHalfy, zbuffer, 1);
	vertexd = glm::vec4(scaledHalfx, -scaledHalfy, zbuffer, 1);
}

void Rectangle::getCoords(float * coords)
{
	unsigned coordCounter = 0;
	for (unsigned i = 0; i < 4; i++) {
		coords[coordCounter] = vertexa[i];
		coordCounter++;
	}
	for (unsigned i = 0; i < 4; i++) {
		coords[coordCounter] = vertexb[i];
		coordCounter++;
	}
	for (unsigned i = 0; i < 4; i++) {
		coords[coordCounter] = vertexc[i];
		coordCounter++;
	}
	for (unsigned i = 0; i < 4; i++) {
		coords[coordCounter] = vertexc[i];
		coordCounter++;
	}
	for (unsigned i = 0; i < 4; i++) {
		coords[coordCounter] = vertexb[i];
		coordCounter++;
	}
	for (unsigned i = 0; i < 4; i++) {
		coords[coordCounter] = vertexd[i];
		coordCounter++;
	}
}

// has to return (2 triangle * 3 vertex * 4 floats) floats
void Rectangle::getColorComponents(float * colorComponents)
{
	for (unsigned i = 0; i < 2 * 3 * 4; i++) {
		colorComponents[i] = color[i%4];
		// if (i % 4 == 0) std::cout << std::endl;
		// std::cout << colorComponents[i] << " ";
	}
}

void Rectangle::getMVPComponents(float * mvpComponents)
{
	unsigned counter = 0;
	for (unsigned i = 0; i < 4; i++) {
		for (unsigned j = 0; j < 4; j++) {
			for (unsigned vertex = 0; vertex < 6; vertex++) {
				mvpComponents[vertex * 16 + counter] = modelMatrix[i][j];
			}
			counter++;
		}
	}
}

glm::mat4 Rectangle::getModel()
{
	return modelMatrix;

}

void Rectangle::updateModel()
{
	// previousTime = currentTime;
	currentTime = glfwGetTime();
	double deltaTime = currentTime - previousTime;
	// double deltaTime = 0.1;
	// std::cout << "ID: " << id << " delta time: " << deltaTime << " [" << previousTime << " : " << currentTime << "]" << std::endl;
	if (deltaTime > 0.5) {
		previousTime = currentTime;
		double speed = 2;
		int translation = static_cast<int>(speed * deltaTime);
		if (!isPinned) {
			switch (spawningSite) {
				case 0:	// from left to right
					modelMatrix = glm::translate(modelMatrix, glm::vec3(translation, 0, 0));
					break;
				case 1:	// from bottom to top
					modelMatrix = glm::translate(modelMatrix, glm::vec3(0, translation, 0));
					break;
				case 2: // from right to left
					modelMatrix = glm::translate(modelMatrix, glm::vec3(-translation, 0, 0));
					break;
				case 3:	// from top to bottom
					modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -translation, 0));
					break;
				default:
					std::cerr << "ERROR: wrong value" << std::endl;

			}
		}
	}
}

glm::vec4 Rectangle::getRandomColor()
{
	enum class Color {RED, BLACK, BLUE, YELLOW};
	Color randomColor = static_cast<Color>(std::rand() * 1.0 / RAND_MAX * 4);
	glm::vec4 color;
	switch (randomColor) {
		case Color::RED:
			color = glm::vec4 (1, 0, 0, 1);
			// std::cout << "red" << std::endl;
			return color;
		case Color::BLACK:
			color = glm::vec4 (0, 0, 0, 1);
			// std::cout << "black" << std::endl;
			return color;
		case Color::BLUE:
			color = glm::vec4 (0, 0, 1, 1);
			// std::cout << "blue" << std::endl;
			return color;
		case Color::YELLOW:
			color = glm::vec4 (1, 1, 0, 1);
			// std::cout << "yellow" << std::endl;
			return color;
		default:
			color = glm::vec4 (0.5, 0.5, 0.5, 1);
			return color;
	}
}

// random proportion sizes between 1 and 10
glm::vec2 Rectangle::getRandomProportion()
{
	int x = static_cast<int>(round((std::rand() * 1.0 / RAND_MAX) * 10 + 1));
	int y = static_cast<int>(round((std::rand() * 1.0 / RAND_MAX) * 10 + 1));
	// TODO: change this lines
	glm::vec2 proportion (x, y);
	// glm::vec2 proportion (1, 1);
	// std::cout << "proportion: " << proportion.x << " " << proportion.y << std::endl;
	return proportion;
}

glm::mat4 Rectangle::getInitialPosition()
{
	spawningSite = (std::rand() * 1.0 / RAND_MAX) * 4;
	int spawningSite2ndComponent = (std::rand() * 1.0 / RAND_MAX) * 20 - 10;
	// std::cout << "spawningSite: " << spawningSite << std::endl;
	// std::cout << "spawningSite2ndComponent: " << spawningSite2ndComponent << std::endl;
	switch (spawningSite) {
		case 0:	// left
			return glm::translate(glm::mat4(1), glm::vec3(-10, spawningSite2ndComponent, 0));
		case 1:	// bottom
			return glm::translate(glm::mat4(1), glm::vec3(spawningSite2ndComponent, -10, 0));
		case 2:	// right
			return glm::translate(glm::mat4(1), glm::vec3(10, spawningSite2ndComponent, 0));
		case 3:	// top
			return glm::translate(glm::mat4(1), glm::vec3(spawningSite2ndComponent, 10, 0));
		default:
			std::cerr << "ERROR: wrong value" << std::endl;
			return glm::translate(glm::mat4(1), glm::vec3(0, 0, 0));
	}
}

bool Rectangle::getIsPinned()
{
	return isPinned;
}

bool Rectangle::shouldBeAlive()
{
	// new coordinates
	glm::vec4 newA = modelMatrix * vertexa;
	glm::vec4 newB = modelMatrix * vertexb;
	glm::vec4 newC = modelMatrix * vertexc;
	glm::vec4 newD = modelMatrix * vertexd;
	switch (spawningSite) {
		case 0:	// from left to right (check for A and C)
			if (newA.x >= 10 || newC.x >= 10) {
				isDead = true;
				return false;
			} else {
				return true;
			}
		case 1:	// from bottom to top (check for C and D)
			if (newC.y >= 10 || newD.y >= 10) {
				isDead = true;
				return false;
			} else {
				return true;
			}
		case 2:	// from right to left (check for B and D)
			if (newB.x <= -10 || newD.x <= -10) {
				isDead = true;
				return false;
			} else {
				return true;
			}
		case 3:	// from top to bottom (check for A and B)
			if (newA.y <= -10 || newB.y <= -10) {
				isDead = true;
				return false;
			} else {
				return true;
			}
		default:
			std::cerr << "ERROR: wrong value" << std::endl;
			return false;
	}
}

bool Rectangle::isAlive()
{
	return !isDead;
}

bool Rectangle::isInside(double x, double y)
{
	// current coordinates
	glm::vec4 currentA = modelMatrix * vertexa;
	glm::vec4 currentB = modelMatrix * vertexb;
	glm::vec4 currentC = modelMatrix * vertexc;
	glm::vec4 currentD = modelMatrix * vertexd;

	// (x,y) should be inside [left, right] and [bottom, top]
	double left, right;
	double bottom, top;

	// L1 vertical line CA
	if (currentA.x == currentC.x) {
		left = currentA.x;
	} else {
		double m1 = (currentC.y - currentA.y) / (currentC.x - currentA.x);
		left = (y - currentA.y) / m1 + currentA.x;
	}

	// L2 verticak line DB
	if (currentD.x == currentB.x) {
		right = currentD.x;
	} else {
		double m2 = (currentD.y - currentB.y) / (currentD.x - currentB.x);
		right = (y - currentB.y) / m2 + currentB.x;
	}

	// L3 horizontal line AB
	// TODO check for case A.x and B.x are equal
	if (currentA.y == currentB.y) {
		top = currentA.y;
	} else {
		double m3 = (currentB.y - currentA.y) / (currentB.x - currentA.x);
		top = m3 * (x - currentA.x) + currentA.y;
	}

	// L4 horizontal line CD
	// TODO check for case C.x and D.x are equal
	if (currentC.y == currentD.y) {
		bottom = currentC.y;
	} else {
		double m4 = (currentD.y - currentC.y) / (currentD.x - currentC.x);
		bottom = m4 * (x - currentC.x) + currentC.y;
	}

	if (x >= left && x <= right && y >= bottom && y <= top) {
		return true;
	} else {
		return false;
	}
}

void Rectangle::checkPinned(double x, double y)
{
	if (!isPinned) {
		// check if (x,y) is inside rectangle ABCD
		if (isInside(x, y)) {
			isPinned = true;
			// std::cout << "pinned!" << std::endl;
		}
	}
}

float Rectangle::getLeft()
{
	glm::vec4 newA = modelMatrix * vertexa;
	return newA.x;
}

float Rectangle::getRight()
{
	glm::vec4 newD = modelMatrix * vertexd;
	return newD.x;
}

float Rectangle::getTop()
{
	glm::vec4 newB = modelMatrix * vertexb;
	return newB.y;
}

float Rectangle::getBottom()
{
	glm::vec4 newC = modelMatrix * vertexc;
	return newC.y;
}
unsigned Rectangle::getID()
{
	return id;
}

glm::vec4 Rectangle::getVertexA()
{
	return vertexa;
}

glm::vec4 Rectangle::getVertexB()
{
	return vertexb;
}

glm::vec4 Rectangle::getVertexC()
{
	return vertexc;
}

glm::vec4 Rectangle::getVertexD()
{
	return vertexd;
}
