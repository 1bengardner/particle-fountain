#include "Particle.h"

vec3D::vec3D() {

}

vec3D::vec3D(float inX, float inY, float inZ) {
	x = inX;
	y = inY;
	z = inZ;
}

void vec3D::add(vec3D inVec) {
	this->x+=inVec.x;
	this->y+=inVec.y;
	this->z+=inVec.z;
}

vec3D vec3D::scalarMultiply(float factor){
	return vec3D(this->x*factor, this->y*factor, this->z*factor);
}

Particle::Particle(vec3D inPosition, vec3D inDirection, float inSpeed, vec3D inAngle, vec3D inRotation, float inSize, vec3D inColour, int inAge, int inShape) {
	position = inPosition;
	direction = inDirection;
	speed = inSpeed;
	angle = inAngle;
	rotation = inRotation;
	size = inSize;
	colour = inColour;
	age = inAge;
	shape = inShape;
}