#include "rectangle.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

const float Rectangle::surfaceSize = 100;
unsigned Rectangle::counter = 0;

Rectangle::Rectangle()
{
	id = counter;
	proportion = glm::vec2(4, 2);
	color = glm::vec4(0, 1, 0, 1);
	position = glm::vec4(0, 0, 0, 1);
	translationMatrix = glm::translate(glm::mat4(1), glm::vec3(1, 0, 0));
	isPinned = false;
	isDead = false;
	counter++;

	// rectangle's vertices
	// float scaleFactor = .5;
	float scaleFactor = 2.0 / surfaceSize;
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
