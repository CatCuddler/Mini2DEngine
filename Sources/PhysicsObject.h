#pragma once

#include "pch.h"

#include <Kore/Graphics1/Image.h>
#include <Kore/Graphics5/Graphics.h>

#include "Collision.h"

// A physically simulated object
class PhysicsObject {
	Kore::vec3 position = Kore::vec3(0, 0, 0);
	
public:
	float Mass;
	Kore::vec3 velocity;
	int id;
	
	static int currentID;
	
	void SetPosition(Kore::vec3 pos) {
		position = pos;
		Collider.center = pos;
	}
	
	Kore::vec3 GetPosition() {
		return position;
	}
	
	// Force accumulator
	Kore::vec3 Accumulator;
	
	SphereCollider Collider;
	
	PhysicsObject();
	
	// Do the integration step for the equations of motion
	void Integrate(float deltaT);
	
	// Apply a force that acts along the center of mass
	void ApplyForceToCenter(Kore::vec3 force);
	
	// Apply an impulse
	void ApplyImpulse(Kore::vec3 impulse);
	
	// Handle the collision with the plane (includes testing for intersection)
	void HandleCollision(const PlaneCollider& collider, float deltaT);
	
	// Handle the collision with another sphere (includes testing for intersection)
	void HandleCollision(PhysicsObject* other, float deltaT);
	
	// Update the matrix of the mesh
	void UpdateMatrix();
	
};
