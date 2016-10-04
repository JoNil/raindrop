#pragma once

#include <vector>
#include "Particle.h"

class ParticleSystem
{
public:
	ParticleSystem();
	ParticleSystem(const int numParticles);
	~ParticleSystem();


	std::vector<Particle> _particles;
};

