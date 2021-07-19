#include "PhysicsObject.h"
#include "Kore/Math/Core.h"
#include "Kore/Log.h"

using namespace Kore;

PhysicsObject::PhysicsObject() {
	Mass = 0;
	position = vec3(0, 0, 0);
	velocity = vec3(0, 0, 0);
	acceleration = vec3(0, 0, 0);
}

void BoxPhysicsObject::HandleCollision(BoxPhysicsObject* other, float deltaT) {
	// Handle the collision between two boxes
	if (collider->IntersectsWith(*other->collider)) {
		//Kore::log(Info, "Colliding with %f %f %f", other->collider->position.x(), other->collider->position.y());
		
		// Calculate the separating velocity
		vec3 collisionNormal = collider->GetCollisionNormal(*other->collider);
		float separatingVelocity = -(other->velocity - velocity) * collisionNormal;
		
		// If we are already separating: Nothing to do
		if (separatingVelocity < 0) return;
		
		float restitution = 0.1f; // bounciness
		
		// Calculate a new one, based on the old one and the restitution
		float newSeparatingVelocity = -separatingVelocity * restitution;
		
		// Move the object out of the collider: The ground is immovable, so we have to move all the way
		float penetrationDepth = -collider->PenetrationDepth(*other->collider);
		position = position - collisionNormal * penetrationDepth;
		collider->position = position;
		
		float deltaVelocity = newSeparatingVelocity - separatingVelocity;
		
		// If the object is very slow, assume resting contact
		/*log(LogLevel::Info, "deltaVelocity %f", deltaVelocity);
		if (deltaVelocity > -10) {
			velocity.set(0, 0, 0);
			//return;
		}*/
		
		// Apply the impulse
		vec3 impulse = collisionNormal * newSeparatingVelocity;
		//vec3 impulse = collisionNormal * deltaVelocity;
		ApplyImpulse(impulse);
	}
}

void SpherePhysicsObject::HandleCollision(SpherePhysicsObject* other, float deltaT) {
	// TODO: Handle the collision between two spheres
	if (collider->IntersectsWith(*other->collider)) {
		
		vec3 collisionNormal = collider->GetCollisionNormal(*other->collider);
		float separatingVelocity = -(other->velocity - velocity) * collisionNormal;
		
		// If we are already separating: Nothing to do
		if (separatingVelocity < 0) return;
		
		float restitution = 0.3f; // bounciness
		
		float newSeparatingVelocity = -separatingVelocity * restitution;
		
		// Move the object out of the collider
		float penetrationDepth = -collider->PenetrationDepth(*other->collider);
		
		// We share the position change equally
		position = position - collisionNormal * penetrationDepth * 0.5f;
		other->position = other->position + collisionNormal * penetrationDepth * 0.5f;
		
		//bool Result = sphereCollider.IntersectsWith(other->sphereCollider);
		
		float deltaVelocity = newSeparatingVelocity - separatingVelocity;
		vec3 impulse = collisionNormal * -deltaVelocity;
		
		ApplyImpulse(-impulse);
		other->ApplyImpulse(impulse);
	}
}

void SpherePhysicsObject::HandleCollision(BoxPhysicsObject* other, float deltaT) {
	// TODO Handle the collision between a box and a spheres
	if (collider->IntersectsWith(*other->collider)) {
		
		vec3 collisionNormal = collider->GetCollisionNormal(*other->collider);
		float separatingVelocity = -(other->velocity - velocity) * collisionNormal;
		
		// If we are already separating: Nothing to do
		if (separatingVelocity < 0) return;
		
		float restitution = 0.3f; // bounciness
		
		float newSeparatingVelocity = -separatingVelocity * restitution;
		
		// Move the object out of the collider
		float penetrationDepth = -collider->PenetrationDepth(*other->collider);
		
		// We share the position change equally
		position = position + collisionNormal * penetrationDepth * 0.5f;
		collider->center = vec3(position.x()+tileWidth/2, position.y()+tileWidth/2, 0);
		other->position = other->position - collisionNormal * penetrationDepth * 0.5f;
		other->collider->position = other->position;
		
		//bool Result = sphereCollider.IntersectsWith(other->sphereCollider);
		
		float deltaVelocity = newSeparatingVelocity - separatingVelocity;
		vec3 impulse = collisionNormal * newSeparatingVelocity;
		//vec3 impulse = collisionNormal * -deltaVelocity;
		ApplyImpulse(impulse);
		other->ApplyImpulse(-impulse);
	}
}

void PhysicsObject::ApplyImpulse(vec3 impulse) {
	velocity += impulse;
}

void PhysicsObject::ApplyForceToCenter(vec3 force) {
	acceleration += force;
}

void PhysicsObject::Integrate(float deltaT) {
	// Derive a new Velocity based on the accumulated forces
	const float damping = 0.9f;
	velocity = velocity + acceleration * deltaT * damping;
	
	// Calculate the new position
	position = position + velocity * deltaT;
	//collider->position = position;
	
	// Clear the accumulator
	acceleration = vec3(0, 0, 0);
}

