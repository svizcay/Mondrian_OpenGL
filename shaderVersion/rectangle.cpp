#include <iostream>
#include <cstdlib>
#include <cmath>
#include <glfw3.h>
#include "rectangle.hpp"

// use extern variables to set up initial parameters

unsigned Rectangle::counter = 0;

Rectangle::Rectangle(int worldWidth, int worldHeight):
	_worldWidth(worldWidth), _worldHeight(worldHeight)
{
	_worldLeft		=	- worldWidth / 2;
	_worldRight		=	  worldWidth / 2;
	_worldBottom	=	- worldHeight / 2;
	_worldTop		=	  worldHeight / 2;

	id = counter;

	// set position, size and color
	position = getInitialPosition();

	// position = glm::vec2(0,0);
	// spawningSite = 0;

	size = getRandomSize();
	color = getRandomColor();			// set it random among fixed values

	isPinned = false;
	isTotallyPositioned = false;
	isDead = false;

	previousTime = glfwGetTime();
	currentTime = glfwGetTime();

	counter++;
}

glm::vec2 Rectangle::getInitialPosition()
{
	// select one among left, bottom, right, top
	spawningSite = (std::rand() * 1.0 / RAND_MAX) * 4;
	// select second component. TODO: include global parameters
	int spawningSite2ndComponent = (std::rand() * 1.0 / RAND_MAX) * 20 - 10;
	// std::cout << "spawningSite: " << spawningSite << std::endl;
	// std::cout << "spawningSite2ndComponent: " << spawningSite2ndComponent << std::endl;
	switch (spawningSite) {
		case 0:	// left
			return glm::vec2(-10, spawningSite2ndComponent);
		case 1:	// bottom
			return glm::vec2(spawningSite2ndComponent, -10);
		case 2:	// right
			return glm::vec2(10, spawningSite2ndComponent);
		case 3:	// top
			return glm::vec2(spawningSite2ndComponent, 10);
		default:
			std::cerr << "ERROR: wrong value" << std::endl;
			return glm::vec2(0, 0);
	}
}

glm::vec2 Rectangle::getRandomSize()
{
	// TODO: include global parameter MAX_SCALE
	const int MAX_SIZE = 5;
	int x = static_cast<int>(round((std::rand() * 1.0 / RAND_MAX) * MAX_SIZE + 1));
	int y = static_cast<int>(round((std::rand() * 1.0 / RAND_MAX) * MAX_SIZE + 1));
	glm::vec2 size (x, y);
	// std::cout << "creating random size: " << size.x << " " << size.y << std::endl;
	return size;
}

glm::vec3 Rectangle::getRandomColor()
{
	enum class Color {RED, BLACK, BLUE, YELLOW};
	Color randomColor = static_cast<Color>(std::rand() * 1.0 / RAND_MAX * 4);
	glm::vec3 color;
	switch (randomColor) {
		case Color::RED:
			color = glm::vec3 (1, 0, 0);
			// std::cout << "red" << std::endl;
			return color;
		case Color::BLACK:
			color = glm::vec3 (0, 0, 0);
			// std::cout << "black" << std::endl;
			return color;
		case Color::BLUE:
			color = glm::vec3 (0, 0, 1);
			// std::cout << "blue" << std::endl;
			return color;
		case Color::YELLOW:
			color = glm::vec3 (1, 1, 0);
			// std::cout << "yellow" << std::endl;
			return color;
		default:
			color = glm::vec3 (0.5, 0.5, 0.5);
			return color;
	}
}

// checkpoint at each .05 value in the grid
void Rectangle::setPositionNearestCheckpoint()
{
	int floor;
	int ceil;

	double checkpointLeft;
	double checkpointRight;

	double distanceLeft;
	double distanceRight;

	switch (spawningSite) {
		case 0:	// from left to right
			{
			floor = std::floor(position.x);
			ceil = std::ceil(position.x);
			if (floor == ceil) {
				position.x = floor + 0.5;

			} else {
				if (position.x - floor > 0.5) {
					checkpointLeft = floor + 0.5;
					checkpointRight = ceil + 0.5;
				} else {
					checkpointLeft = floor - 0.5;
					checkpointRight = floor + 0.5;
				}
				distanceLeft = position.x - checkpointLeft;
				distanceRight = checkpointRight - position.x;
				if (distanceLeft < distanceRight) {
					position.x = checkpointLeft;
				} else {
					position.x = checkpointRight;
				}
			}
			break;
			}
		case 1:	// from bottom to top
			{
			floor = std::floor(position.y);
			ceil = std::ceil(position.y);
			if (floor == ceil) {
				position.y = floor + 0.5;

			} else {
				if (position.y - floor > 0.5) {
					checkpointLeft = floor + 0.5;
					checkpointRight = ceil + 0.5;
				} else {
					checkpointLeft = floor - 0.5;
					checkpointRight = floor + 0.5;
				}
				distanceLeft = position.y - checkpointLeft;
				distanceRight = checkpointRight - position.y;
				if (distanceLeft < distanceRight) {
					position.y = checkpointLeft;
				} else {
					position.y = checkpointRight;
				}
			}
			break;
			}
		case 2: // from right to left
			{
			floor = std::floor(position.x);
			ceil = std::ceil(position.x);
			if (floor == ceil) {
				position.x = floor - 0.5;
			} else {
				if (ceil - position.x > 0.5) {
					checkpointLeft = floor - 0.5;
					checkpointRight = ceil - 0.5;
				} else {
					checkpointLeft = ceil - 0.5;
					checkpointRight = ceil + 0.5;
				}
				distanceLeft = position.x - checkpointLeft;
				distanceRight = checkpointRight - position.x;
				if (distanceLeft < distanceRight) {
					position.x = checkpointLeft;
				} else {
					position.x = checkpointRight;
				}
			}
			break;
			}
		case 3:	// from top to bottom
			{
			floor = std::floor(position.y);
			ceil = std::ceil(position.y);
			if (floor == ceil) {
				position.y = floor - 0.5;
			} else {
				if (ceil - position.y > 0.5) {
					checkpointLeft = floor - 0.5;
					checkpointRight = ceil - 0.5;
				} else {
					checkpointLeft = ceil - 0.5;
					checkpointRight = ceil + 0.5;
				}
				distanceLeft = position.y - checkpointLeft;
				distanceRight = checkpointRight - position.y;
				if (distanceLeft < distanceRight) {
					position.y = checkpointLeft;
				} else {
					position.y = checkpointRight;
				}
			}
			break;
			}
		default:
			std::cerr << "ERROR: wrong value" << std::endl;

	}
}

void Rectangle::updatePosition()
{

	currentTime = glfwGetTime();
	double deltaTime = currentTime - previousTime;

	/*
	 * continuous
	 */
	previousTime = currentTime;
	// TODO: make speed a static variable
	double speed = 7.5;
	double translation = speed * deltaTime;
	if (!isTotallyPositioned) {
		// rectangle is still moving
		if (!isPinned) {
			// use normal speed
			switch (spawningSite) {
				case 0:	// from left to right
					position += glm::vec2(translation, 0);
					break;
				case 1:	// from bottom to top
					position += glm::vec2(0, translation);
					break;
				case 2: // from right to left
					position += glm::vec2(-translation, 0);
					break;
				case 3:	// from top to bottom
					position += glm::vec2(0, -translation);
					break;
				default:
					std::cerr << "ERROR: wrong value" << std::endl;

			}
		} else {
			// rectangle was pinned but needs to go to its final position
			isTotallyPositioned = true;
			setPositionNearestCheckpoint();
		}
	}
}

void Rectangle::tryToKill()
{
	// TODO: change hardcoded values
	glm::vec2 halfSize = size / 2.0f;
	GLfloat left	=	(position - halfSize).x;
	GLfloat right	=	(position + halfSize).x;
	GLfloat bottom	=	(position - halfSize).y;
	GLfloat top		=	(position + halfSize).y;

	switch (spawningSite) {
		case 0:	// from left to right (check for A and C)
			if (left > 10) {
				isDead = true;
			}
			break;
		case 1:	// from bottom to top (check for C and D)
			if (bottom > 10) {
				isDead = true;
			}
			break;
		case 2:	// from right to left (check for B and D)
			if (right < -10) {
				isDead = true;
			}
			break;
		case 3:	// from top to bottom (check for A and B)
			if (top < -10) {
				isDead = true;
			}
			break;
		default:
			std::cerr << "ERROR: wrong value in Rectangle::tryToKill()" << std::endl;
	}
}

glm::vec2 Rectangle::getPosition()
{
	return position;
}

glm::vec2 Rectangle::getSize()
{
	return size;
}

glm::vec3 Rectangle::getColor()
{
	return color;
}

bool Rectangle::isAlive()
{
	return !isDead;
}

void Rectangle::checkPinned(double worldx, double worldy)
{
	if (!isPinned) {
		// check if (x,y) is inside rectangle ABCD
		if (isInside(worldx, worldy)) {
			isPinned = true;
		}
	}
}

bool Rectangle::isInside(double worldx, double worldy)
{
	glm::vec2 halfSize = size / 2.0f;
	GLfloat left	=	(position - halfSize).x;
	GLfloat right	=	(position + halfSize).x;
	GLfloat bottom	=	(position - halfSize).y;
	GLfloat top		=	(position + halfSize).y;

	bool inRow = false;
	bool inCol = false;

	if (left <= worldx && worldx <= right) {
		inCol = true;
	}

	if (bottom <= worldy && worldy <= top) {
		inRow = true;
	}

	return (inRow && inCol);
}
