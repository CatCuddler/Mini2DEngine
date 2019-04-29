#pragma once

#include "pch.h"

class SphereCollider;

// A plane is defined as the plane's normal and the distance of the plane to the origin
class PlaneCollider {
public:
	float d;
	Kore::vec3 normal;
};

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
		if(position.x() < other.position.x() + other.width &&
		   position.x() + width > other.position.x() &&
		   position.y() < other.position.y() + other.height &&
		   position.y() + height > other.position.y()) return true;
	 
		return false;
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
	
	bool IntersectsWith(const PlaneCollider& other) {
		return other.normal.dot(center) + other.d <= radius;
	}
	
	Kore::vec3 GetCollisionNormal(const PlaneCollider& other) {
		return other.normal;
	}
	
	float PenetrationDepth(const PlaneCollider &other) {
		return other.normal.dot(center) + other.d - radius;
	}
	
};
