#include "ParticleSystem.h"



ParticleSystem::ParticleSystem()
{
}


ParticleSystem::ParticleSystem(const int numParticles)
{
	_particles.resize(numParticles);
	for (int i = 0; i < (int)_particles.size(); ++i) {
		_particles[i]._pos.x = (float)(std::rand() % 200) / 100.0f - 1;
		_particles[i]._pos.y = (float)(std::rand() % 200) / 100.0f - 1;
		_particles[i]._size = (float)(std::rand() % 60) / 1000.0f + 0.03;
	}
}

ParticleSystem::~ParticleSystem()
{
}
