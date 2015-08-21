#ifndef RECTANGLES_HPP
#define RECTANGLES_HPP

#include <vector>
#include <map>
#include <GL/glew.h>
#include "rectangle.hpp"

class Rectangles {
public:
	Rectangles(
			int worldWidth, int worldHeight,
			unsigned maxNrRectangles, unsigned maxSize,
			unsigned invalidPosition,
			double lineThickness
	);

	~Rectangles();

	void createOne();
	void update();
	void getPositions(GLfloat *positions);
	void getColors(GLfloat *colors);
	void getSizes(GLfloat *sizes);
	int size();
	void finish();
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

	// end simulation: draw lines
	bool ended;
	unsigned nrLines;
	GLfloat *linesPositions;
	GLfloat *linesSizes;
	double _LINE_THICKNESS;

	bool *animationStarted;
	double *animationStart;

	// animate rectangle onClick
	std::map<unsigned, double>onClickAnimationStart;	// key: rectangle's id; value: start time

};

#endif
