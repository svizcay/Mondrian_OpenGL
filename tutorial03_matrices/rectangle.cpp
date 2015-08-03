#include "rectangle.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>
#include <ctime>

const float Rectangle::surfaceSize = 1;
unsigned Rectangle::counter = 0;

Rectangle::Rectangle()
{
	id = counter;
	proportion = getRandomProportion();	// set it random
	color = getRandomColor();			// set it random among fixed values

	position = glm::vec4(0, 0, 0, 1);

	scaleMatrix = glm::scale(glm::mat4(1), glm::vec3(0.5, 0.5, 1));
	rotationMatrix = glm::rotate(glm::mat4(1), 0.0f, glm::vec3(0, 0, 1));
	translationMatrix = getInitialPosition();
	// modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
	// TODO change next lines
	modelMatrix = translationMatrix * rotationMatrix * scaleMatrix * glm::mat4(1);
	// modelMatrix = translationMatrix * glm::mat4(1);

	isPinned = false;
	isDead = false;
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
	vertexa = glm::vec4(-scaledHalfx, scaledHalfy, 0, 1);
	vertexb = glm::vec4(scaledHalfx, scaledHalfy, 0, 1);
	vertexc = glm::vec4(-scaledHalfx, -scaledHalfy, 0, 1);
	vertexd = glm::vec4(scaledHalfx, -scaledHalfy, 0, 1);
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

glm::mat4 Rectangle::getModel()
{
	return modelMatrix;

}

void Rectangle::updateModel()
{
	switch (spawningSite) {
		case 0:	// from left to right
			modelMatrix = glm::translate(modelMatrix, glm::vec3(1, 0, 0));
			break;
		case 1:	// from bottom to top
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1, 0));
			break;
		case 2: // from right to left
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-1, 0, 0));
			break;
		case 3:	// from top to bottom
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -1, 0));
			break;
		default:
			std::cerr << "ERROR: wrong value" << std::endl;

	}
}

glm::vec4 Rectangle::getRandomColor()
{
	float red = std::rand() * 1.0 / RAND_MAX;
	float green = std::rand() * 1.0 / RAND_MAX;
	float blue = std::rand() * 1.0 / RAND_MAX;
	glm::vec4 color (red, green, blue, 1);
	// std::cout << color.x << " " << color.y << " " << color.z << std::endl;
	return color;
}

// random proportion sizes between 1 and 10
glm::vec2 Rectangle::getRandomProportion()
{
	float x = (std::rand() * 1.0 / RAND_MAX) * 10 + 1;
	float y = (std::rand() * 1.0 / RAND_MAX) * 10 + 1;
	// TODO: change this lines
	glm::vec2 proportion (x, y);
	// glm::vec2 proportion (1, 1);
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
