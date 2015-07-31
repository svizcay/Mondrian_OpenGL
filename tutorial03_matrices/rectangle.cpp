#include "rectangle.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

const float Rectangle::surfaceSize = 1;
unsigned Rectangle::counter = 0;

Rectangle::Rectangle()
{
	id = counter;
	proportion = glm::vec2(5, 1);			// set it random
	color = glm::vec4(0, 1, 0, 1);			// set it random among fixed values

	position = glm::vec4(0, 0, 0, 1);

	scaleMatrix = glm::scale(glm::mat4(1), glm::vec3(1, 1, 1));
	rotationMatrix = glm::rotate(glm::mat4(1), 0.0f, glm::vec3(0, 0, 1));
	translationMatrix = glm::translate(glm::mat4(1), glm::vec3(-10, 0, 0));
	// modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
	modelMatrix = translationMatrix * rotationMatrix * scaleMatrix * glm::mat4(1);

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
