#include <Kore/Graphics1/Color.h>
#include <Kore/Log.h>

#include "PhysicsWorld.h"

using namespace Kore;

namespace {
	int arraySize;
}

PhysicsWorld::PhysicsWorld(BoxPhysicsObject* player) : player(player) {
	arraySize = mapRows * mapColumns;
	boxPhysicsObjects = new BoxPhysicsObject*[arraySize];
	spherePhysicsObjects = new SpherePhysicsObject*[arraySize];
	for (int i = 0; i < arraySize; i++) {
		boxPhysicsObjects[i] = nullptr;
		spherePhysicsObjects[i] = nullptr;
	}
}

PhysicsWorld::~PhysicsWorld() {
	boxPhysicsObjects = nullptr;
	spherePhysicsObjects = nullptr;
}

void PhysicsWorld::Update(float deltaT) {
	if (player != nullptr) {
		// Apply gravity (= constant accceleration, so we multiply with the mass and divide in the integration step.
		// The alternative would be to add gravity during the integration as a constant.
		
		const float gravity = 9.81;
		player->ApplyForceToCenter(vec3(0, player->Mass * gravity, 0));
		
		// Integrate the equations of motion
		player->Integrate(deltaT);
	}
	
	HandleCollisions(deltaT);
}

void PhysicsWorld::HandleCollisions(float deltaT) {
	// Check for collisions with box colliders
	if (player != nullptr) {
		BoxPhysicsObject** currentCollision = &boxPhysicsObjects[0];
		while (*currentCollision != nullptr) {
			player->HandleCollision(*currentCollision, deltaT);
			++currentCollision;
		}
	}
	
	// Check for collisions with sphere colliders
	/*if (player != nullptr) {
		SpherePhysicsObject** currentCollision = &spherePhysicsObjects[0];
		while (*currentCollision != nullptr) {
			(*currentCollision)->HandleCollision(player, deltaT);
			++currentCollision;
		}
	}*/
}

void PhysicsWorld::AddObject(BoxPhysicsObject* bc) {
	int counter = 0;
	BoxPhysicsObject** current = &boxPhysicsObjects[0];
	while (*current != nullptr) {
		assert(counter < arraySize);
		++current;
		++counter;
	}
	*current = bc;
}

void PhysicsWorld::AddObject(SpherePhysicsObject* sc) {
	int counter = 0;
	SpherePhysicsObject** current = &spherePhysicsObjects[0];
	while (*current != nullptr) {
		assert(counter < arraySize);
		++current;
		++counter;
	}
	*current = sc;
}

void PhysicsWorld::DrawBoundingBox(Kore::Graphics2::Graphics2* g2) {
	g2->setColor(Graphics1::Color::Yellow);
	
	BoxPhysicsObject** bc = &boxPhysicsObjects[0];
	while (*bc != nullptr) {
		vec3 position = (*bc)->collider->position;
		g2->drawRect(position.x(), position.y(), (*bc)->collider->width, (*bc)->collider->height);
		++bc;
	}
	
	g2->setColor(Graphics1::Color::Red);
	if (player != nullptr) {
		vec3 position = player->collider->position;
		g2->drawRect(position.x(), position.y(), player->collider->width, player->collider->height);
	}
	
	g2->setColor(Graphics1::Color::White);
}

void PhysicsWorld::DrawBoundingSphere(Kore::Graphics2::Graphics2* g2) {
	g2->setFontColor(Graphics1::Color::Red);
	
	SpherePhysicsObject** sc = &spherePhysicsObjects[0];
	while (*sc != nullptr) {
		vec3 center = (*sc)->collider->center;
		g2->drawString("*", center.x(), center.y());
		++sc;
	}
	
	g2->setFontColor(Graphics1::Color::White);
}
