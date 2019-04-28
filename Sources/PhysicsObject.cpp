#include "PhysicsObject.h"
#include "Kore/Log.h"

using namespace Kore;

int PhysicsObject::currentID;

PhysicsObject::PhysicsObject() {
	Accumulator = vec3(0, 0, 0);
	velocity = vec3(0, 0, 0);
	sphereCollider.radius = 0.1f;
	id = ++currentID;
}

void PhysicsObject::HandleCollision(const PlaneCollider& collider, float deltaT) {
	// Check if we are colliding with the plane
	if (sphereCollider.IntersectsWith(collider)) {
		
		// Kore::log(Info, "Floor");
		float restitution = 0.8f;
		
		// Calculate the separating velocity
		float separatingVelocity = -(collider.normal * velocity);
		
		if (separatingVelocity < 0) return;
		
		// Calculate a new one, based on the old one and the restitution
		float newSeparatingVelocity = -separatingVelocity * restitution;
		
		
		// Move the object out of the collider
		float penetrationDepth = sphereCollider.PenetrationDepth(collider);
		//Position += collider.normal * -penetrationDepth;
		//SetPosition(Position);
		
		bool Result = sphereCollider.IntersectsWith(collider);
		
		// Calculate the impulse
		// The plane is immovable, so we have to move all the way
		float deltaVelocity = newSeparatingVelocity - separatingVelocity;
		
		// If the object is very slow, assume resting contact
		if (deltaVelocity > -1.5f) {
			velocity.set(0, 0, 0);
			position = vec3(position.x(), sphereCollider.radius - collider.d, position.z());
			sphereCollider.center = position;
			return;
		}
		
		// Apply the impulse
		vec3 impulse = collider.normal * -deltaVelocity;
		
		ApplyImpulse(impulse);
	}
}

void PhysicsObject::HandleCollision(BoxCollider* collider, float deltaT) {
	// Check if we are colliding with the ground
	if (sphereCollider.IntersectsWith(*collider)) {
		
		// Kore::log(Info, "Floor");
		float restitution = 0.8f;
		
		// Calculate the separating velocity
		vec3 collisionNormal = sphereCollider.GetCollisionNormal(*collider);
		float separatingVelocity = -(collisionNormal * velocity);
		
		if (separatingVelocity < 0) return;
		
		// Calculate a new one, based on the old one and the restitution
		float newSeparatingVelocity = -separatingVelocity * restitution;
		
		
		// Move the object out of the collider
		float penetrationDepth = sphereCollider.PenetrationDepth(*collider);
		//Position += collider.normal * -penetrationDepth;
		//SetPosition(Position);
		
		bool Result = sphereCollider.IntersectsWith(*collider);
		
		// Calculate the impulse
		// The plane is immovable, so we have to move all the way
		float deltaVelocity = newSeparatingVelocity - separatingVelocity;
		
		// If the object is very slow, assume resting contact
		if (deltaVelocity > -1.5f) {
			velocity.set(0, 0, 0);
			position = vec3(position.x(), sphereCollider.radius /*- collider->d*/, position.z());
			sphereCollider.center = position;
			return;
		}
		
		// Apply the impulse
		vec3 impulse = collisionNormal * -deltaVelocity;
		
		ApplyImpulse(impulse);
	}
}

void PhysicsObject::HandleCollision(PhysicsObject* other, float deltaT) {
	// Handle the collision with another sphere
	if (sphereCollider.IntersectsWith(other->sphereCollider)) {
		
		//Kore::log(Info, "Intersection");
		
		float restitution = 0.8f;
		
		vec3 collisionNormal = sphereCollider.GetCollisionNormal(other->sphereCollider);
		
		float separatingVelocity = -(other->velocity - velocity) * collisionNormal;
		
		// If we are already separating: Nothing to do
		if (separatingVelocity < 0) return;
		
		float newSeparatingVelocity = -separatingVelocity * restitution;
		
		float deltaVelocity = newSeparatingVelocity - separatingVelocity;
		
		// Move the object out of the collider
		float penetrationDepth = -sphereCollider.PenetrationDepth(other->sphereCollider);
		
		// We share the position change equally
		SetPosition(position + collisionNormal * penetrationDepth * 0.5f);
		other->SetPosition(other->position - collisionNormal * penetrationDepth * 0.5f);
		
		bool Result = sphereCollider.IntersectsWith(other->sphereCollider);
		
		vec3 impulse = collisionNormal * -deltaVelocity;
		
		ApplyImpulse(-impulse);
		other->ApplyImpulse(impulse);
	}
}


void PhysicsObject::ApplyImpulse(vec3 impulse) {
	velocity += impulse;
}

void PhysicsObject::ApplyForceToCenter(vec3 force) {
	Accumulator += force;
}

void PhysicsObject::Integrate(float deltaT) {
	// Derive a new position based on the velocity (Note: Use SetPosition to also set the collider's values)
	SetPosition(position + velocity * deltaT);
	
	// Derive a new Velocity based on the accumulated forces
	velocity += (Accumulator / Mass) * deltaT;
	
	// Multiply by a damping coefficient
	float damping = 0.9f;
	velocity *= damping;
	
	// Clear the accumulator
	Accumulator = vec3(0, 0, 0);
}

