#pragma once

#include <Kore/Graphics4/Graphics.h>
#include <Kore/Graphics2/Graphics.h>

#include "Collision.h"

  class PhysicsObject;

// Handles all physically simulated objects.
class PhysicsWorld {
public:
	
	// Null terminated array
	PhysicsObject** physicsObjects;
	BoxCollider** boxColliders;
	
	PhysicsWorld();
	
	// Integration step
	void Update(float deltaT);
	
	// Handle the collisions
	void HandleCollisions(float deltaT);
	
	// Add an object to be simulated
	void AddObject(PhysicsObject* po);
	void AddObject(BoxCollider* bc);
	
	void DrawBoundingBox(Kore::Graphics2::Graphics2* g2);
};
