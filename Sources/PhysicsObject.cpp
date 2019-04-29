#include "PhysicsObject.h"
#include "Kore/Log.h"

using namespace Kore;

int PhysicsObject::currentID;

PhysicsObject::PhysicsObject() {
	Accumulator = vec3(0, 0, 0);
	velocity = vec3(0, 0, 0);
	collider.radius = 0.1f;
	id = ++currentID;
}

void PhysicsObject::HandleCollision(const PlaneCollider& planeCollider, float deltaT) {
	// Check if we are colliding with the plane
	if (collider.IntersectsWith(planeCollider)) {
		
		// Kore::log(Info, "Floor");
		float restitution = 0.8f;
		
		// Calculate the separating velocity
		float separatingVelocity = -(planeCollider.normal * velocity);
		
		if (separatingVelocity < 0) return;
		
		// Calculate a new one, based on the old one and the restitution
		float newSeparatingVelocity = -separatingVelocity * restitution;
		
		
		// Move the object out of the collider
		float penetrationDepth = collider.PenetrationDepth(planeCollider);
		//Position += collider.normal * -penetrationDepth;
		//SetPosition(Position);
		
		//bool Result = sphereCollider.IntersectsWith(collider);
		
		// Calculate the impulse
		// The plane is immovable, so we have to move all the way
		float deltaVelocity = newSeparatingVelocity - separatingVelocity;
		
		// If the object is very slow, assume resting contact
		if (deltaVelocity > -1.5f) {
			velocity.set(0, 0, 0);
			position = vec3(position.x(), collider.radius - planeCollider.d, position.z());
			collider.center = position;
			return;
		}
		
		// Apply the impulse
		vec3 impulse = planeCollider.normal * -deltaVelocity;
		
		ApplyImpulse(impulse);
	}
}

void PhysicsObject::HandleCollision(BoxCollider* boxCollider, float deltaT) {
	// Check if we are colliding with the ground
	if (collider.IntersectsWith(*boxCollider)) {
		//Kore::log(Info, "Floor");
		float restitution = 0.8f;
		
		// Calculate the separating velocity
		vec3 collisionNormal = collider.GetCollisionNormal(*boxCollider);
		float separatingVelocity = velocity * collisionNormal;
		
		// If we are already separating: Nothing to do
		if (separatingVelocity < 0) return;
		
		// Calculate a new one, based on the old one and the restitution
		float newSeparatingVelocity = -separatingVelocity * restitution;
		
		float deltaVelocity = newSeparatingVelocity - separatingVelocity;
		
		// Calculate the impulse
		// The ground is immovable, so we have to move all the way
		float penetrationDepth = -collider.PenetrationDepth(*boxCollider);
//		SetPosition(position + collisionNormal * -penetrationDepth);
		
		//bool Result = sphereCollider.IntersectsWith(other->sphereCollider);
		
		// If the object is very slow, assume resting contact
		if (deltaVelocity > -1.5f) {
			velocity.set(0, 0, 0);
			//position = vec3(position.x(), sphereCollider.radius - collider.d, position.z());
			//sphereCollider.center = position;
			return;
		}
		
		// Apply the impulse
		vec3 impulse = collisionNormal * -deltaVelocity;
		ApplyImpulse(-impulse);
	}
}

void PhysicsObject::HandleCollision(PhysicsObject* otherCollider, float deltaT) {
	// Handle the collision with another sphere
	if (collider.IntersectsWith(otherCollider->collider)) {
		
		//Kore::log(Info, "Intersection");
		
		float restitution = 0.8f;
		
		vec3 collisionNormal = collider.GetCollisionNormal(otherCollider->collider);
		
		float separatingVelocity = -(otherCollider->velocity - velocity) * collisionNormal;
		
		// If we are already separating: Nothing to do
		if (separatingVelocity < 0) return;
		
		float newSeparatingVelocity = -separatingVelocity * restitution;
		
		float deltaVelocity = newSeparatingVelocity - separatingVelocity;
		
		// Move the object out of the collider
		float penetrationDepth = -collider.PenetrationDepth(otherCollider->collider);
		
		// We share the position change equally
		SetPosition(position + collisionNormal * penetrationDepth * 0.5f);
		otherCollider->SetPosition(otherCollider->position - collisionNormal * penetrationDepth * 0.5f);
		
		//bool Result = sphereCollider.IntersectsWith(other->sphereCollider);
		
		vec3 impulse = collisionNormal * -deltaVelocity;
		
		ApplyImpulse(-impulse);
		otherCollider->ApplyImpulse(impulse);
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

