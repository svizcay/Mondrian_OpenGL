#include "rectangles.hpp"
#include <iostream>

Rectangles::Rectangles(
			int worldWidth, int worldHeight,
			unsigned maxNrRectangles, unsigned maxSize,
			unsigned invalidPosition) : 
	_worldWidth(worldWidth), _worldHeight(worldHeight),
	_MAX_NR_RECTANGLES(maxNrRectangles), _MAX_SIZE(maxSize),
	_INVALID_POSITION(invalidPosition)
{
	// world space
	_worldLeft		=	- worldWidth / 2;
	_worldRight		=	  worldWidth / 2;
	_worldBottom	=	- worldHeight / 2;
	_worldTop		=	  worldHeight / 2;
}

void Rectangles::createOne()
{
	if (rectangles.size() < _MAX_NR_RECTANGLES) {
		// std::cout << "inserting a new rectangle" << std::endl;
		Rectangle rectangle (_worldWidth, _worldHeight);
		rectangles.push_back(rectangle);
	}
}

void Rectangles::update()
{
	// update position for each retangle
	for(unsigned i = 0; i < rectangles.size(); i++) {
		rectangles[i].updatePosition();
		rectangles[i].tryToKill();
	}

	// delete rectangles that are not visible anymore
	for (std::vector<Rectangle>::iterator it = rectangles.begin(); it != rectangles.end();) {
		if (it->isAlive()) {
			it++;
		} else {
			it = rectangles.erase(it);
		}
	}
}

void Rectangles::getPositions(GLfloat *positions)
{
	// std::cout << "positions: " << std::endl;
	unsigned counter = 0;
	for(unsigned i = 0; i < rectangles.size(); i++) {
		glm::vec2 position = rectangles[i].getPosition();
		// std::cout << position.x << " " << position.y << std::endl;
		positions[counter++] = position.x;
		positions[counter++] = position.y;
	}

	// add an invalid position so fragment shader doesn't go through the whole array
	if (rectangles.size() < _MAX_NR_RECTANGLES) {
		positions[counter++] = _INVALID_POSITION;
		positions[counter++] = _INVALID_POSITION;
	}
}

void Rectangles::getColors(GLfloat *colors)
{
	unsigned counter = 0;
	for(unsigned i = 0; i < rectangles.size(); i++) {
		glm::vec3 color = rectangles[i].getColor();
		colors[counter++] = color.r;
		colors[counter++] = color.g;
		colors[counter++] = color.b;
	}
}

void Rectangles::getSizes(GLfloat *sizes)
{
	unsigned counter = 0;
	for(unsigned i = 0; i < rectangles.size(); i++) {
		glm::vec2 size = rectangles[i].getSize();
		sizes[counter++] = size.x;
		sizes[counter++] = size.y;
	}
}

int Rectangles::size()
{
	return rectangles.size();
}
