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

	// each edge might generate 2 lines
	linesPositions = new GLfloat[maxNrRectangles * 4 * 2];
	linesSizes = new GLfloat[maxNrRectangles * 4 * 2];

	animationStarted = NULL;
	animationStart = NULL;

}

Rectangles::~Rectangles() {
	delete [] linesPositions;
	delete [] linesSizes;

	delete [] animationStarted;
	delete [] animationStart;
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
		// double animationScale = speed * elapsedTime;
		// if (animationScale > 1) animationScale = 1;

		for (unsigned i = 0; i < nrLines; i++) {
			if (elapsedTime > i / 4.0) {
				if (!animationStarted[i]) {
					animationStart[i] = glfwGetTime();
					animationStarted[i] = true;
				}

				double animationElapsedTime = glfwGetTime() - animationStart[i];
				double animationScale = speed * animationElapsedTime;
				if (animationScale > 1) animationScale = 1;

				sizes[counter++] = linesSizes[i*2+0] * animationScale;
				sizes[counter++] = linesSizes[i*2+1] * animationScale;

			} else {
				sizes[counter++] = 0;
				sizes[counter++] = 0;
			}
		}
	}

	for(unsigned i = 0; i < rectangles.size(); i++) {
		if (rectangles[i].getIsJustPinned()) {
			onClickAnimationStart[rectangles[i].getID()] = glfwGetTime();
			rectangles[i].animationAlreadyStarted();
		}

		double animationScale = 1;
		double ANIMATION_MAX_TIME = 0.3;
		double ANIMATION_MAX_SCALE = 1.1;
		double ANIMATION_HALF_TIME = ANIMATION_MAX_TIME / 2.0;

		if (onClickAnimationStart.find(rectangles[i].getID()) != onClickAnimationStart.end()) {
			// animation started
			double elapsedTime = glfwGetTime() - onClickAnimationStart[rectangles[i].getID()];
			if (elapsedTime < ANIMATION_HALF_TIME) {
				animationScale = elapsedTime * (ANIMATION_MAX_SCALE - 1) / ANIMATION_HALF_TIME + 1;
			} else if (elapsedTime >= ANIMATION_HALF_TIME && elapsedTime < ANIMATION_MAX_TIME) {
				animationScale = (ANIMATION_MAX_SCALE - 1) * (elapsedTime - ANIMATION_MAX_TIME) / (ANIMATION_HALF_TIME - ANIMATION_MAX_TIME) + 1;
			} else {
				animationScale = 1;
			}

		}
		glm::vec2 size = rectangles[i].getSize();
		sizes[counter++] = size.x * animationScale;
		sizes[counter++] = size.y * animationScale;
	}
}

void Rectangles::getIsPinned(GLint *isPinned)
{
	unsigned counter = 0;

	// if ended add lines
	if (ended) {
		for (unsigned i = 0; i < nrLines; i++) {
			isPinned[counter++] = static_cast<GLint>(PinnedStatus::NOT_RECTANGLE);
		}
	}

	for(unsigned i = 0; i < rectangles.size(); i++) {
		if (rectangles[i].getIsPinned()) {
			isPinned[counter++] = static_cast<GLint>(PinnedStatus::PINNED);
		} else {
			isPinned[counter++] = static_cast<GLint>(PinnedStatus::NOT_PINNED);
		}
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

void Rectangles::createVerticalLine(unsigned rectangleIndex, glm::vec2 linePosition)
{

	// line's top, bottom, left, right
	float left		= -10;
	float right		=  10;
	float bottom	= -10;
	float top		=  10;

	float rectangleLeft		= rectangles[rectangleIndex].getLeft();
	float rectangleRight	= rectangles[rectangleIndex].getRight();
	float rectangleBottom	= rectangles[rectangleIndex].getBottom();
	float rectangleTop		= rectangles[rectangleIndex].getTop();

	for (unsigned j = 0; j < rectangles.size(); j++) {
		if (j != rectangleIndex && rectangles[j].getIsPinned()) {
			float otherRectangleLeft	= rectangles[j].getLeft();
			float otherRectangleRight	= rectangles[j].getRight();
			float otherRectangleBottom	= rectangles[j].getBottom();
			float otherRectangleTop		= rectangles[j].getTop();

			// check collision between rectangles
			if (rectangles[rectangleIndex].checkCollision(rectangles[j])) {
				if (rectangles[rectangleIndex].getID() < rectangles[j].getID()) {
					// rectangle j is behind current rectangle -> do nothing

				} else {
					// rectangle j is over current rectangle

					if (otherRectangleLeft < linePosition.x && linePosition.x < otherRectangleRight) {
						// left edge collision

						// check how many lines should be drawn
						if (rectangleBottom < otherRectangleBottom && rectangleTop > otherRectangleTop) {
							// draw two lines
							std::cout << "drawing two lines!!" << std::endl;
							// TODO: write this case
							// currently drawing full line

						} else if (rectangleTop > otherRectangleTop) {
							// draw one line
							bottom = otherRectangleTop;
						} else if (rectangleBottom < otherRectangleBottom) {
							// draw one line
							top = otherRectangleBottom;
						} else {
							// current edge is totally behind another rectangle
							// draw nothing
							// bottom = 10;
							// top = -10;
							return;
						}

					}
				}
			} else {
				// no collision between rectangles rectangleID and j
				if (otherRectangleLeft < linePosition.x && linePosition.x < otherRectangleRight) {
					// collision between edge and rectangle j
					if (otherRectangleBottom >= rectangleTop && otherRectangleBottom < top) top = otherRectangleBottom;
					if (otherRectangleTop <= rectangleBottom && otherRectangleTop > bottom) bottom = otherRectangleTop;
				}

			}
		}
	}

	addLine(linePosition, bottom, top, true);
}

void Rectangles::createHorizontalLine(unsigned rectangleIndex, glm::vec2 linePosition)
{

	// line's top, bottom, left, right
	float left		= -10;
	float right		=  10;
	float bottom	= -10;
	float top		=  10;

	float rectangleLeft		= rectangles[rectangleIndex].getLeft();
	float rectangleRight	= rectangles[rectangleIndex].getRight();
	float rectangleBottom	= rectangles[rectangleIndex].getBottom();
	float rectangleTop		= rectangles[rectangleIndex].getTop();

	for (unsigned j = 0; j < rectangles.size(); j++) {
		if (j != rectangleIndex && rectangles[j].getIsPinned()) {
			float otherRectangleLeft	= rectangles[j].getLeft();
			float otherRectangleRight	= rectangles[j].getRight();
			float otherRectangleBottom	= rectangles[j].getBottom();
			float otherRectangleTop		= rectangles[j].getTop();

			// check collision between rectangles
			if (rectangles[rectangleIndex].checkCollision(rectangles[j])) {
				if (rectangles[rectangleIndex].getID() < rectangles[j].getID()) {
					// rectangle j is behind current rectangle -> do nothing

				} else {
					// rectangle j is over current rectangle

					if (otherRectangleBottom < linePosition.y && linePosition.y < otherRectangleTop) {
						// left edge collision

						// check how many lines should be drawn
						if (rectangleLeft < otherRectangleLeft && rectangleRight > otherRectangleRight) {
							// draw two lines
							std::cout << "drawing two lines!!" << std::endl;
							// TODO: write this case
							// currently drawing full line

						} else if (rectangleRight > otherRectangleRight) {
							// draw one line
							left = otherRectangleRight;
						} else if (rectangleLeft < otherRectangleLeft) {
							// draw one line
							right = otherRectangleLeft;
						} else {
							// current edge is totally behind another rectangle
							// draw nothing
							// left = 10;
							// right = -10;
							return;
						}

					}
				}
			} else {
				// no collision between rectangles rectangleID and j
				if (otherRectangleBottom < linePosition.y && linePosition.y < otherRectangleTop) {
					// collision between edge and rectangle j
					if (otherRectangleLeft >= rectangleRight && otherRectangleLeft < right) right = otherRectangleLeft;
					if (otherRectangleRight <= rectangleLeft && otherRectangleRight > left) left = otherRectangleRight;
				}

			}
		}
	}

	addLine(linePosition, left, right, false);
}

// edge id: 0 = left; 1 = right; 2 = bottom; 3 = top
void Rectangles::createLines(unsigned rectangleIndex, Edge edgeID)
{

	// line's center
	glm::vec2 linePosition = glm::vec2(0.0f, 0.0f);

	if (edgeID == Edge::LEFT || edgeID == Edge::RIGHT) {
		// vertical edge
		
		if (edgeID == Edge::LEFT) {
			// left edge
			linePosition.x = rectangles[rectangleIndex].getLeft();
		} else {
			// right edge
			linePosition.x = rectangles[rectangleIndex].getRight();
		}
		createVerticalLine(rectangleIndex, linePosition);

	} else {
		// horizontal edge

		if (edgeID == Edge::BOTTOM) {
			// bottom edge
			linePosition.y = rectangles[rectangleIndex].getBottom();
		} else {
			// top edge
			linePosition.y = rectangles[rectangleIndex].getTop();
		}
		createHorizontalLine(rectangleIndex, linePosition);
	}

}

void Rectangles::addLine(glm::vec2 linePosition, GLfloat inf, GLfloat sup, bool isVertical)
{
	if (isVertical) {
		linesPositions[nrLines * 2 + 0] = linePosition.x;
		linesPositions[nrLines * 2 + 1] = (sup + inf) / 2;

		linesSizes[nrLines * 2 + 0] = _LINE_THICKNESS;
		linesSizes[nrLines * 2 + 1] = (sup - inf);
	} else {
		linesPositions[nrLines * 2 + 0] = (sup + inf) / 2;
		linesPositions[nrLines * 2 + 1] = linePosition.y;

		linesSizes[nrLines * 2 + 0] = (sup - inf);
		linesSizes[nrLines * 2 + 1] = _LINE_THICKNESS;
	}

	nrLines++;
}

void Rectangles::finish()
{
	ended = true;

	// calculate lines positions, sizes and nrLines
	for(unsigned i = 0; i < rectangles.size(); i++) {

		if (rectangles[i].getIsPinned()) {

			// create edges (0:left, 1:right, 2:bottom, 3:top)

			createLines(i, Edge::LEFT);
			createLines(i, Edge::RIGHT);
			createLines(i, Edge::BOTTOM);
			createLines(i, Edge::TOP);

		}
	}

	std::cout << "nr lines: " << nrLines << std::endl;

	animationStarted = new bool[nrLines];
	animationStart = new double[nrLines];
	for (unsigned i = 0; i < nrLines; i++) {
		animationStarted[i] = false;
	}
}

