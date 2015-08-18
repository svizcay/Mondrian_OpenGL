#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include <glm/glm.hpp>

class Rectangle {
public:
	Rectangle(int worldWidth, int worldHeight);

	void updatePosition();
	glm::vec2 getPosition();
	glm::vec2 getSize();
	glm::vec3 getColor();
	bool isAlive();
	void tryToKill();
	void checkPinned(double worldx, double worldy);

private:
	// global parameters
	int _worldWidth;
	int _worldHeight;
	float _worldLeft;
	float _worldRight;
	float _worldBottom;
	float _worldTop;

	static unsigned counter;
	
	unsigned id;
	glm::vec2 position;
	glm::vec2 size;
	glm::vec3 color;

	unsigned spawningSite;			// 0:left; 1:botton; 2:right; 3:top

	bool isPinned;
	bool isTotallyPositioned;
	bool isDead;

	glm::vec3 getRandomColor();
	glm::vec2 getRandomSize();
	glm::vec2 getInitialPosition();
	void setPositionNearestCheckpoint();

	bool isInside(double worldx, double worldy);

	/*
	bool isInside(double x, double y);
	*/

	// timers
	double previousTime;
	double currentTime;

};

#endif
