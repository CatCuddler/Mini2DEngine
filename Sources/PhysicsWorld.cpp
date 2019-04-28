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
	for (int i = 0; i < 100; i++) {
		physicsObjects[i] = nullptr;
		boxColliders[i] = nullptr;
	}
}

void PhysicsWorld::Update(float deltaT) {
	PhysicsObject** currentP = &physicsObjects[0];
	while (*currentP != nullptr) {
		// Apply gravity (= constant accceleration, so we multiply with the mass and divide in the integration step.
		// The alternative would be to add gravity during the integration as a constant.
		
		(*currentP)->ApplyForceToCenter(vec3(0, (*currentP)->Mass * 9.81, 0));
		
		// Integrate the equations of motion
		(*currentP)->Integrate(deltaT);
		++currentP;
	}
	
	// Check for collisions with the plane
	/*currentP = &physicsObjects[0];
	while (*currentP != nullptr) {
		//(*currentP)->HandleCollision(plane, deltaT);
		++currentP;
	}*/
	
	// Check for collisions with the other objects
	currentP = &physicsObjects[0];
	while (*currentP != nullptr) {
		PhysicsObject** currentCollision = currentP + 1;
		while (*currentCollision != nullptr) {
			(*currentP)->HandleCollision(*currentCollision, deltaT);
			++currentCollision;
		}
		
		BoxCollider** bc = &boxColliders[0];
		while (*bc != nullptr) {
			(*currentP)->HandleCollision(*bc, deltaT);
			++bc;
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
	
	PhysicsObject** po = &physicsObjects[0];
	while (*po != nullptr) {
		g2->setColor(Graphics1::Color::Red);
		
		vec3 center = (*po)->sphereCollider.center;
		float radius = (*po)->sphereCollider.radius;
		g2->drawRect(center.x() - radius, center.y() - radius, radius * 2, radius * 2);
	
		++po;
	}
	
	BoxCollider** bc = &boxColliders[0];
	while (*bc != nullptr) {
		g2->setColor(Graphics1::Color::White);
		
		vec3 position = (*bc)->position;
		float width = (*bc)->width;
		float height = (*bc)->height;
		g2->drawRect(position.x(), position.y(), width, height);
		
		++bc;
	}
	
}
