#pragma once

#include <vector>
#include "Particle.h"

class ParticleSystem
{
public:
	ParticleSystem();
	ParticleSystem(const int numParticles, Particle *player, glm::vec2 wind);
	~ParticleSystem();
	void simulate_particles(float dt);

	std::vector<Particle> _particles;
	Particle *_player;
	glm::vec2 _wind;
};

