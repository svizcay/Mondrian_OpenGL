#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include <glm/glm.hpp>

class Rectangle {
private:
	static const float surfaceSize;
	static unsigned counter;
	unsigned id;
	glm::vec2 proportion;
	glm::vec4 color;

	// translate * rotate * scale * original_value
	glm::vec4 position;				// ? maybe not required
	unsigned spawningSite;			// 0:left; 1:botton; 2:right; 3:top
	glm::mat4 translationMatrix;
	glm::mat4 rotationMatrix;
	glm::mat4 scaleMatrix;
	// model = translate * rotate * scale
	glm::mat4 modelMatrix;

	double zbuffer;

	bool isPinned;
	bool isDead;

	// object space [-1:1]
	glm::vec4 vertexa;
	glm::vec4 vertexb;
	glm::vec4 vertexc;
	glm::vec4 vertexd;

	glm::vec4 getRandomColor();
	glm::vec2 getRandomProportion();
	glm::mat4 getInitialPosition();
	bool isInside(double x, double y);

	// timers
	double previousTime;
	double currentTime;
public:
	Rectangle();

	void getCoords(float * coords);
	void getColorComponents(float * colorComponents);
	void getMVPComponents(float * mvpComponents);
	glm::mat4 getModel();
	void updateModel();
	bool shouldBeAlive();
	bool isAlive();
	bool getIsPinned();
	void checkPinned(double x, double y);

	float getLeft();
	float getRight();
	float getTop();
	float getBottom();
	unsigned getID();

	glm::vec4 getVertexA();
	glm::vec4 getVertexB();
	glm::vec4 getVertexC();
	glm::vec4 getVertexD();
};

#endif
