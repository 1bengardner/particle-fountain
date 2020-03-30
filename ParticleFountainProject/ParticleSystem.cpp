#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(float o [3]) {
	origin[3] = o [3];
	std::vector<Particle*> particles;
};

void ParticleSystem::addParticle(Particle * p) {
	particles.push_back(p);
}

void ParticleSystem::removeParticle(int index) {
	if (particles.size() > 0) {
		particles.pop_back();
	}
}