#include "rectangles.hpp"
#include <iostream>
#include <GL/glew.h>
#include <glfw3.h>

Rectangles::Rectangles(
			int worldWidth, int worldHeight,
			unsigned maxNrRectangles, unsigned maxSize,
			unsigned invalidPosition, double lineThickness) : 
	_worldWidth(worldWidth), _worldHeight(worldHeight),
	_MAX_NR_RECTANGLES(maxNrRectangles), _MAX_SIZE(maxSize),
	_INVALID_POSITION(invalidPosition),
	_LINE_THICKNESS(lineThickness)
{
	// world space
	_worldLeft		=	- worldWidth / 2;
	_worldRight		=	  worldWidth / 2;
	_worldBottom	=	- worldHeight / 2;
	_worldTop		=	  worldHeight / 2;

	ended = false;
	nrLines = 0;

	linesPositions = new GLfloat[maxNrRectangles * 4];
	linesSizes = new GLfloat[maxNrRectangles * 4];

}

Rectangles::~Rectangles() {
	delete [] linesPositions;
	delete [] linesSizes;
}

void Rectangles::createOne()
{
	if (rectangles.size() < _MAX_NR_RECTANGLES / 5) {
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
	unsigned counter = 0;

	// if ended add lines
	if (ended) {
		for (unsigned i = 0; i < nrLines; i++) {
			positions[counter++] = linesPositions[i*2+0];
			positions[counter++] = linesPositions[i*2+1];
		}
	}

	for(unsigned i = 0; i < rectangles.size(); i++) {
		glm::vec2 position = rectangles[i].getPosition();
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

	// if ended add lines
	if (ended) {
		for (unsigned i = 0; i < nrLines; i++) {
			colors[counter++] = 0.0f;
			colors[counter++] = 0.0f;
			colors[counter++] = 0.0f;
		}
	}

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

	// if ended add lines
	if (ended) {
		static double timeStart = glfwGetTime();
		double timeEnd = glfwGetTime();
		double elapsedTime = timeEnd - timeStart;
		double speed = 1;
		double animationScale = speed * elapsedTime;
		if (animationScale > 1) animationScale = 1;

		for (unsigned i = 0; i < nrLines; i++) {
			sizes[counter++] = linesSizes[i*2+0] * animationScale;
			sizes[counter++] = linesSizes[i*2+1] * animationScale;
		}
	}

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

void Rectangles::checkPinned(double worldx, double worldy)
{
	for (unsigned i = 0; i < rectangles.size(); i++) {
		rectangles[i].checkPinned(worldx, worldy);
	}
}

void Rectangles::finish()
{
	ended = true;
	// calculate lines positions, sizes and nrLines
	unsigned counter = 0;
	for(unsigned i = 0; i < rectangles.size(); i++) {

		if (rectangles[i].getIsPinned()) {

			// line top bottom left and right
			float top, bottom, left, right;
			// line position
			glm::vec2 position;

			// for each vertical edge
			glm::vec2 rectanglePosition = rectangles[i].getPosition();

			// left edge
			position.x = rectangles[i].getLeft();
			top = 10;
			bottom = -10;
			for (unsigned j = 0; j < rectangles.size(); j++) {
				if (j != i && rectangles[j].getIsPinned()) {
					float otherRectangleLeft = rectangles[j].getLeft();
					float otherRectangleRight = rectangles[j].getRight();
					float otherRectangleBottom = rectangles[j].getBottom();
					float otherRectangleTop = rectangles[j].getTop();
					if (otherRectangleLeft < position.x && position.x < otherRectangleRight) {
						if (otherRectangleBottom > rectanglePosition.y && otherRectangleBottom < top) top = otherRectangleBottom;
						if (otherRectangleTop < rectanglePosition.y && otherRectangleTop > bottom) bottom = otherRectangleTop;
					}
				}
			}
			linesPositions[counter] = position.x;
			linesSizes[counter] = _LINE_THICKNESS;
			counter++;
			linesPositions[counter] = (top + bottom) / 2;
			linesSizes[counter] = (top - bottom);
			counter++;
			nrLines++;

			// right edge
			position.x = rectangles[i].getRight();
			top = 10;
			bottom = -10;
			for (unsigned j = 0; j < rectangles.size(); j++) {
				if (j != i && rectangles[j].getIsPinned()) {
					float otherRectangleLeft = rectangles[j].getLeft();
					float otherRectangleRight = rectangles[j].getRight();
					float otherRectangleBottom = rectangles[j].getBottom();
					float otherRectangleTop = rectangles[j].getTop();
					if (otherRectangleLeft < position.x && position.x < otherRectangleRight) {
						if (otherRectangleBottom > rectanglePosition.y && otherRectangleBottom < top) top = otherRectangleBottom;
						if (otherRectangleTop < rectanglePosition.y && otherRectangleTop > bottom) bottom = otherRectangleTop;
					}
				}
			}
			linesPositions[counter] = position.x;
			linesSizes[counter] = _LINE_THICKNESS;
			counter++;
			linesPositions[counter] = (top + bottom) / 2;
			linesSizes[counter] = (top - bottom);
			counter++;
			nrLines++;

			// for each horizontal edge

			// bottom edge
			position.y = rectangles[i].getBottom();
			left = -10;
			right = 10;
			for (unsigned j = 0; j < rectangles.size(); j++) {
				if (j != i && rectangles[j].getIsPinned()) {
					float otherRectangleLeft = rectangles[j].getLeft();
					float otherRectangleRight = rectangles[j].getRight();
					float otherRectangleBottom = rectangles[j].getBottom();
					float otherRectangleTop = rectangles[j].getTop();
					if (otherRectangleBottom < position.y && position.y < otherRectangleTop) {
						if (otherRectangleLeft > rectanglePosition.x && otherRectangleLeft < right) right = otherRectangleLeft;
						if (otherRectangleRight < rectanglePosition.x && otherRectangleRight > left) left = otherRectangleRight;
					}
				}
			}
			linesPositions[counter] = (right + left) / 2;
			linesSizes[counter] = (right - left);
			counter++;
			linesPositions[counter] = position.y;
			linesSizes[counter] = _LINE_THICKNESS;
			counter++;
			nrLines++;

			// top edge
			position.y = rectangles[i].getTop();
			left = -10;
			right = 10;
			for (unsigned j = 0; j < rectangles.size(); j++) {
				if (j != i && rectangles[j].getIsPinned()) {
					float otherRectangleLeft = rectangles[j].getLeft();
					float otherRectangleRight = rectangles[j].getRight();
					float otherRectangleBottom = rectangles[j].getBottom();
					float otherRectangleTop = rectangles[j].getTop();
					if (otherRectangleBottom < position.y && position.y < otherRectangleTop) {
						if (otherRectangleLeft > rectanglePosition.x && otherRectangleLeft < right) right = otherRectangleLeft;
						if (otherRectangleRight < rectanglePosition.x && otherRectangleRight > left) left = otherRectangleRight;
					}
				}
			}
			linesPositions[counter] = (right + left) / 2;
			linesSizes[counter] = (right - left);
			counter++;
			linesPositions[counter] = position.y;
			linesSizes[counter] = _LINE_THICKNESS;
			counter++;
			nrLines++;
		}
	}
}

