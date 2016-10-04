#include "ParticleSystem.h"
#include <glm/glm.hpp>


ParticleSystem::ParticleSystem()
{
}


ParticleSystem::ParticleSystem(const int numParticles, Particle *player, glm::vec2 wind)
	: _player(player)
	, _wind(wind)
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

void ParticleSystem::simulate_particles(float dt)
{
	Particle * particles = _particles.data();
	int particle_count = _particles.size();

	for (int i = 0; i < particle_count; ++i) {

		//keep in bounderies around the player
		if (particles[i]._pos.y < _player->_pos.y - 2) {
			particles[i]._size = 0;
		}
		else if (particles[i]._pos.x < _player->_pos.x - 2) {
			particles[i]._size = 0;
		}

		//respawn dead
		if (particles[i]._size == 0) {
			//this particle was absorbed or reached the boundary
			//and should be reinitialized

			//        ______
			//       /      |
			//    /  |   1  |
			// /___2_|______|
			// |     | 3   /
			// |  G  |   /
			// |_____|/
			if ((float)(std::rand() % 2) >= 1) {
				particles[i]._pos.x = (float)(std::rand() % 200) / 100.0f + 1;
				particles[i]._pos.y = (float)(std::rand() % 200) / 100.0f + 1;
			}
			else
			{
				particles[i]._pos.x = (float)(std::rand() % 200) / 100.0f - 1;
				particles[i]._pos.y = (float)(std::rand() % 200) / 100.0f - 1;

				if (particles[i]._pos.y > particles[i]._pos.x) {
					particles[i]._pos.x += 2.0f;
				}
				else {
					particles[i]._pos.y += 2.0f;
				}
			}
			particles[i]._size = (float)(std::rand() % 60) / 1000.0f + 0.03;
		}

		//simulation
		particles[i].physics_step(_wind, dt);

		//collision
		for (int j = i + 1; j < particle_count; j++)
		{
			float dist = glm::length(particles[j]._pos - particles[i]._pos);
			//std::cout << "dist " << dist << "\n";
			if (dist < particles[j]._size + particles[i]._size) {
				//std::cout << "collision\n";
				//collision detected
				if (particles[j]._size <= particles[i]._size)
				{
					//i absorbes j
					//calculate new radius for new area
					particles[i].particle_collision_response(&particles[j]);
				}
				else {
					//j absorbes i
					//calculate new radius for new area
					particles[j].particle_collision_response(&particles[i]);
				}
			}
		}
	}
}