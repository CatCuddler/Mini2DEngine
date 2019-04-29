#include "pch.h"

#include <Kore/Graphics1/Color.h>

#include "PhysicsWorld.h"
#include "PhysicsObject.h"

using namespace Kore;

namespace {
	const int arraySize = 100;
}

PhysicsWorld::PhysicsWorld() {
	physicsObjects = new PhysicsObject*[arraySize];
	boxColliders = new BoxCollider*[arraySize];
	for (int i = 0; i < arraySize; i++) {
		physicsObjects[i] = nullptr;
		boxColliders[i] = nullptr;
	}
}

void PhysicsWorld::Update(float deltaT) {
	PhysicsObject** currentP = &physicsObjects[0];
	while (*currentP != nullptr) {
		// Apply gravity (= constant accceleration, so we multiply with the mass and divide in the integration step.
		// The alternative would be to add gravity during the integration as a constant.
		
		float gravity = 9.81 * 100;
		(*currentP)->ApplyForceToCenter(vec3(0, (*currentP)->Mass * gravity, 0));
		
		// Integrate the equations of motion
		(*currentP)->Integrate(deltaT);
		++currentP;
	}
	
	// Check for collisions with the ground
	currentP = &physicsObjects[0];
	while (*currentP != nullptr) {
		BoxCollider** bc = &boxColliders[0];
		while (*bc != nullptr) {
			(*currentP)->HandleCollision(*bc, deltaT);
			++bc;
		}
		++currentP;
	}
	
	// Check for collisions with the other physics objects
	currentP = &physicsObjects[0];
	while (*currentP != nullptr) {
		PhysicsObject** currentCollision = currentP + 1;
		while (*currentCollision != nullptr) {
			(*currentP)->HandleCollision(*currentCollision, deltaT);
			++currentCollision;
		}
		
		++currentP;
	}
}


void PhysicsWorld::AddObject(PhysicsObject* po) {
	int counter = 0;
	PhysicsObject** current = &physicsObjects[0];
	while (*current != nullptr) {
		assert(counter < arraySize);
		++current;
		++counter;
	}
	*current = po;
}

void PhysicsWorld::AddObject(BoxCollider* bc) {
	int counter = 0;
	BoxCollider** current = &boxColliders[0];
	while (*current != nullptr) {
		assert(counter < arraySize);
		++current;
		++counter;
	}
	*current = bc;
}

void PhysicsWorld::DrawBoundingBox(Kore::Graphics2::Graphics2* g2) {
	g2->setColor(Graphics1::Color::Red);
	PhysicsObject** po = &physicsObjects[0];
	while (*po != nullptr) {
		vec3 position = (*po)->collider.position;
		g2->drawRect(position.x(), position.y(), (*po)->collider.width, (*po)->collider.height);
		++po;
	}
	
	g2->setColor(Graphics1::Color::Yellow);
	BoxCollider** bc = &boxColliders[0];
	while (*bc != nullptr) {
		vec3 position = (*bc)->position;
		g2->drawRect(position.x(), position.y(), (*bc)->width, (*bc)->height);
		++bc;
	}
	
	g2->setColor(Graphics1::Color::White);
}
