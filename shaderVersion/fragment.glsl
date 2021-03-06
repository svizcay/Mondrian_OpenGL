#version 330 core

#define MAX_NR_RECTANGLES 100
#define INVALID_POSITION 100
#define MAX_SIMULATION_TIME 15
#define MAX_TIME_TRANSITION_FINISH_GAME 2;

layout(origin_upper_left) in vec4 gl_FragCoord;

// Ouput data
out vec4 color;

uniform vec2 positions[MAX_NR_RECTANGLES];
uniform vec2 sizes[MAX_NR_RECTANGLES];
uniform vec3 colors[MAX_NR_RECTANGLES];
uniform int isPinned[MAX_NR_RECTANGLES];
uniform vec2 windowSize;

uniform vec2 timers;

uniform sampler2D tex;

// /*
//    gl_FragCoord location of the fragment in window's space
//    origin (0,0) in bottom left unless user declare it otherwise
//    top left = (0,height)
//    bottom right = (width, 0)
// 
//    changing origin to top left:
//    layout(origin_upper_left) in vec4 gl_FragCoord;
// */

void main()
{
	bool isValidRow;
	bool isValidCol;

	int validID = -1;

	vec2 texCoord = vec2(0.5, 0.5);

	for (int i = 0; i < MAX_NR_RECTANGLES; i++) {

		// check if it's a valid position, ie: rectangle is alive
		// 100 = invalid position
		if (positions[i].x == INVALID_POSITION || positions[i].y == INVALID_POSITION) {
			break;
		}

		// vertices in model space ([-1:1])
		vec2 vertexA = vec2(-0.5,  0.5);	// top left
		vec2 vertexB = vec2( 0.5,  0.5);	// top right
		vec2 vertexC = vec2(-0.5, -0.5);	// bottom left
		vec2 vertexD = vec2( 0.5, -0.5);	// bottom right

		// scale
		vertexA *= sizes[i];
		vertexB *= sizes[i];
		vertexC *= sizes[i];
		vertexD *= sizes[i];

		// translate
		vertexA += positions[i].xy;
		vertexB += positions[i].xy;
		vertexC += positions[i].xy;
		vertexD += positions[i].xy;

		// orthographic projection
		// it will be drawn only if vertex is inside [-1:1]
		vertexA /= 10;
		vertexB /= 10;
		vertexC /= 10;
		vertexD /= 10;

		// form ndc to windows coordinates (origin top left)
		vertexA.x = (vertexA.x + 1) * windowSize.x / 2;
		vertexB.x = (vertexB.x + 1) * windowSize.x / 2;
		vertexC.x = (vertexC.x + 1) * windowSize.x / 2;
		vertexD.x = (vertexD.x + 1) * windowSize.x / 2;

		vertexA.y = (1 - vertexA.y) * windowSize.y / 2;
		vertexB.y = (1 - vertexB.y) * windowSize.y / 2;
		vertexC.y = (1 - vertexC.y) * windowSize.y / 2;
		vertexD.y = (1 - vertexD.y) * windowSize.y / 2;

		isValidRow = gl_FragCoord.x >= vertexA.x && gl_FragCoord.x <= vertexB.x;
		isValidCol = gl_FragCoord.y <= vertexC.y && gl_FragCoord.y >= vertexA.y;

		if (isValidRow && isValidCol) {
			validID = i;
			texCoord.x = (gl_FragCoord.x - vertexA.x) / (vertexB.x - vertexA.x);
			texCoord.y = 1 - (gl_FragCoord.y - vertexA.y) / (vertexC.y - vertexA.y);
			break;
		}
	}

	float deltaTime = timers.y - timers.x;		// current time - start time

	if (validID != -1) {
		// color = vec4(colors[validID].rgb, 1);
		// color = vec4(colors[validID].r, colors[validID].g, colors[validID].b, 1);
		// color = vec4(1, 1, 1, 1) * vec4(colors[validID], 1);
		if (deltaTime > MAX_SIMULATION_TIME) {
			color = vec4(colors[validID], 1);
		} else {
			if (isPinned[validID] == 1) {
				if (colors[validID] == vec3(0, 0, 0)) {
					color = texture(tex, texCoord) * vec4(1, 1, 1, 1);
				} else {
					color = texture(tex, texCoord) * vec4(colors[validID], 1);
				}
			} else {
				color = texture(tex, texCoord) * vec4(0.3, 0.3, 0.3, 1);
			}
		}
	} else {
		// background color
		if (deltaTime > MAX_SIMULATION_TIME) {
			float gray = (deltaTime - MAX_SIMULATION_TIME) / MAX_TIME_TRANSITION_FINISH_GAME;
			if (gray > 1) gray = 1;
			color = vec4(gray, gray, gray, 1);
		} else {
			color = vec4(0, 0, 0, 1);
		}
	}
}
