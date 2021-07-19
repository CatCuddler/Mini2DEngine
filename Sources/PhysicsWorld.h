#pragma once

#include <Kore/Graphics2/Graphics.h>

#include "PhysicsObject.h"
#include "Collision.h"

class BoxPhysicsObject;
class SpherePhysicsObject;

// Handles all physically simulated objects.
class PhysicsWorld {
public:
	
	// Null terminated array
	BoxPhysicsObject* player;
	BoxPhysicsObject** boxPhysicsObjects;
	SpherePhysicsObject** spherePhysicsObjects;
	
	PhysicsWorld(BoxPhysicsObject* player);
	~PhysicsWorld();
	
	// Integration step
	void Update(float deltaT);
	
	// Handle the collisions
	void HandleCollisions(float deltaT);
	
	// Add an object to be simulated
	void AddObject(BoxPhysicsObject* bc);
	void AddObject(SpherePhysicsObject* bc);
	
	void DrawBoundingBox(Kore::Graphics2::Graphics2* g2);
	void DrawBoundingSphere(Kore::Graphics2::Graphics2* g2);
};
