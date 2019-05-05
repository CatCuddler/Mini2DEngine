#pragma once

#include "pch.h"

class SphereCollider;

// A box is defined by width and height and position (top left)
class BoxCollider {
public:
	Kore::vec3 position;
	float width, height;
	
	// Collision normal is the normal vector pointing towards the other sphere
	Kore::vec3 GetCollisionNormal(const BoxCollider& other) {
		Kore::vec3 n = other.position - position;
		n = n.normalize();
		return n;
	}
	
	// The penetration depth
	float PenetrationDepth(const BoxCollider& other) {
		return width - (other.position - position).getLength();
	}
	
	// Return true iff there is an intersection with the other box
	bool IntersectsWith(const BoxCollider& other) {
		return position.x() < other.position.x() + other.width &&
		position.x() + width > other.position.x() &&
		position.y() < other.position.y() + other.height &&
		position.y() + height > other.position.y();
	}
	
};

// A sphere is defined by a radius and a center.
class SphereCollider {
public:
	Kore::vec3 center;
	float radius;
	
	// Return true iff there is an intersection with the other sphere
	bool IntersectsWith(const SphereCollider& other) {
		return PenetrationDepth(other) > 0.0f;
	}
	
	// Collision normal is the normal vector pointing towards the other sphere
	Kore::vec3 GetCollisionNormal(const SphereCollider& other) {
		Kore::vec3 n = other.center - center;
		n = n.normalize();
		return n;
	}
	
	// The penetration depth
	float PenetrationDepth(const SphereCollider& other) {
		return other.radius + radius - (other.center - center).getLength();
	}
};
