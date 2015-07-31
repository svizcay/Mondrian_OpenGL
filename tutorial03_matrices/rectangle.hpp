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
	glm::mat4 translationMatrix;
	glm::mat4 rotationMatrix;
	glm::mat4 scaleMatrix;
	// model = translate * rotate * scale
	glm::mat4 modelMatrix;

	bool isPinned;
	bool isDead;

	// object space [-1:1]
	glm::vec4 vertexa;
	glm::vec4 vertexb;
	glm::vec4 vertexc;
	glm::vec4 vertexd;

	glm::vec4 getRandomColor();

public:
	Rectangle();

	void getCoords(float * coords);
	void getColorComponents(float * colorComponents);
	glm::mat4 getModel();

};

#endif
