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
	glm::vec4 position;
	glm::mat4 translationMatrix;
	bool isPinned;
	bool isDead;

	glm::vec4 vertexa;
	glm::vec4 vertexb;
	glm::vec4 vertexc;
	glm::vec4 vertexd;

public:
	Rectangle();

	void getCoords(float * coords);
	void getColorComponents(float * colorComponents);

};

#endif
