#pragma once

#include "PhysicsWorld.h"
#include "Collision.h"
#include "Settings.h"

// A physically simulated object -> the player
class PhysicsObject {
public:
	float Mass;
	Kore::vec3 position;
	Kore::vec3 velocity;
	Kore::vec3 acceleration;
	
	PhysicsObject();
	
	// Do the integration step for the equations of motion
	void Integrate(float deltaT);
	
	// Apply a force that acts along the center of mass
	void ApplyForceToCenter(Kore::vec3 force);
	
	// Apply an impulse
	void ApplyImpulse(Kore::vec3 impulse);
};

class BoxPhysicsObject : public PhysicsObject {
public:
	BoxCollider* collider;
	
	BoxPhysicsObject() : PhysicsObject() {
		collider = new BoxCollider();
		collider->position = position;
		collider->width = tileWidth;
		collider->height = tileHeight;
	}
	
	~BoxPhysicsObject() {
		collider = nullptr;
	}
	
	// Handle collisions
	void HandleCollision(BoxPhysicsObject* other, float deltaT);
	
	void Integrate(float deltaT) {
		PhysicsObject::Integrate(deltaT);
		collider->position = position;
	}
};

class SpherePhysicsObject : public PhysicsObject {
public:
	SphereCollider* collider;
	
	SpherePhysicsObject() : PhysicsObject() {
		collider = new SphereCollider();
		collider->center = Kore::vec3(position.x()+tileWidth/2, position.y()+tileHeight/2, 0);
		collider->radius = tileWidth/2;
	}
	
	~SpherePhysicsObject() {
		collider = nullptr;
	}
	
	// Handle collisions
	void HandleCollision(SpherePhysicsObject* other, float deltaT);
	void HandleCollision(BoxPhysicsObject* other, float deltaT);
	
	void Integrate(float deltaT) {
		PhysicsObject::Integrate(deltaT);
		collider->center = Kore::vec3(position.x()+tileWidth/2, position.y()+tileHeight/2, 0);
	}
};
