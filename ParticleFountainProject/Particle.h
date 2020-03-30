#ifndef PARTICLE_H
#define PARTICLE_H

class vec3D {
public:
	vec3D();
	vec3D(float inX, float inY, float inZ);
	float x;
	float y;
	float z;

	void add(vec3D inVec);
	vec3D scalarMultiply(float factor);
};

class Particle {
public:
	Particle(vec3D inPosition, vec3D inDirection, float inSpeed, vec3D inAngle, vec3D inRotation, float inSize, vec3D inColour, int inAge, int inShape);
	vec3D position;
	vec3D direction;
	float speed;
	vec3D angle;
	vec3D rotation;
	float size;
	vec3D colour; // colour defined as a vec3D of R, G, B from 0-1
	int age;
	int shape; // cube = 1, sphere = 2
};

#endif