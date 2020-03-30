#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <vector>
#include "Particle.h"

class ParticleSystem {
private:
	float origin [3];
public:
	ParticleSystem(float origin [3]);
	std::vector<Particle*> particles;
	void addParticle(Particle *p);
	void removeParticle(int index);
};

#endif