#pragma once

#include <glm/vec2.hpp>

class Particle {

public:
	Particle() {}

	glm::vec2 _pos;
	glm::vec2 _speed;
	glm::vec2 _acc;
	float _size;
};