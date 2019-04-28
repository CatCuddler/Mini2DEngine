#pragma once

#include "pch.h"

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
	
	// Return true iff there is an intersection with the other box
	bool IntersectsWith(const BoxCollider& other) {
		// AABB 1
		int xMin1 = position.x();
		int xMax1 = position.x() + width;
		int yMax1 = position.y() + height;
		int yMin1 = position.y();
		
		// AABB 2
		int xMin2 = other.position.x();
		int xMax2 = other.position.x() + other.width;
		int yMax2 = other.position.y() + other.height;
		int yMin2 = other.position.y();
		
		// Collision tests
		if( xMax1 < xMin2 || xMin1 > xMax2 ) return false;
		if( yMax1 < yMin2 || yMin1 > yMax2 ) return false;
		
		return true;
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
