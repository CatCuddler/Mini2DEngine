#pragma once

#include <Kore/Graphics4/Graphics.h>
#include "Collision.h"

  class PhysicsObject;

// Handles all physically simulated objects.
class PhysicsWorld {
public:
	
	// The ground
	BoxCollider** groundObjects;
	
	// Null terminated array of PhysicsObject pointers b
	PhysicsObject** physicsObjects;
	
	PhysicsWorld();
	
	// Integration step
	void Update(float deltaT);
	
	// Handle the collisions
	void HandleCollisions(float deltaT);
	
	// Add an object to be simulated
	void AddObject(PhysicsObject* po);
	
};
