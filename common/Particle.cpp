#include "Particle.h"
#include <iostream> //sqrtf ??

void Particle::physics_step(glm::vec2 external_force, float dt)
{
	_acc = (external_force / (50.0f * sqrtf(_size)));
	_acc -= _speed;
	_speed += _acc * dt;
	_pos += _speed * dt;
}

void Particle::particle_collision_response(Particle * collider)
{
	float old_size = _size;

	_size = sqrt(_size * _size + collider->_size * collider->_size);
	_pos = (old_size * _pos + collider->_size * collider->_pos) / (old_size + collider->_size);
	_speed = (old_size * _speed + collider->_size * collider->_speed) / (old_size + collider->_size);

	collider->_pos = glm::vec2(0.0f, 0.0f);
	collider->_speed = glm::vec2(0.0f, 0.0f);
	collider->_size = 0.0f;
}