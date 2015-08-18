#ifndef RECTANGLES_HPP
#define RECTANGLES_HPP

#include <vector>
#include <GL/glew.h>
#include "rectangle.hpp"

class Rectangles {
public:
	Rectangles(
			int worldWidth, int worldHeight,
			unsigned maxNrRectangles, unsigned maxSize,
			unsigned invalidPosition
	);

	void createOne();
	void update();
	void getPositions(GLfloat *positions);
	void getColors(GLfloat *colors);
	void getSizes(GLfloat *sizes);
	int size();
	void checkPinned(double worldx, double worldy);

private:
	// world space
	int _worldWidth;
	int _worldHeight;
	float _worldLeft;
	float _worldRight;
	float _worldBottom;
	float _worldTop;

	// limits
	unsigned _MAX_NR_RECTANGLES;
	unsigned _MAX_SIZE;
	unsigned _INVALID_POSITION;

	// inner storage
	std::vector<Rectangle> rectangles;

};

#endif
