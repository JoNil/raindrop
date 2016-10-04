#pragma once

#include <glm/vec2.hpp>

class Particle {

public:
	Particle() {}

	void physics_step(glm::vec2 external_force, float dt);

	void particle_collision_response(Particle * collider);

	glm::vec2 _pos;
	glm::vec2 _speed;
	glm::vec2 _acc;
	float _size;
};