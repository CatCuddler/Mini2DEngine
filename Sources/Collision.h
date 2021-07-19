#pragma once

#include "Settings.h"
#include "Kore/Math/Core.h"

// A box is defined by width and height and position (top left)
class BoxCollider {
public:
	Kore::vec3 position;
	float width, height;
	
	// Collision normal is the normal vector pointing towards the other collider
	Kore::vec3 GetCollisionNormal(const BoxCollider& other) {
		Kore::vec3 n = other.position - position;
		n = n.normalize();
		return n;
	}
	
	// The penetration depth
	float PenetrationDepth(const BoxCollider& other) {
		Kore::vec3 center = Kore::vec3(position.x() + width/2, position.y() + height/2, 0);
		Kore::vec3 centerOther = Kore::vec3(other.position.x() + other.width/2, other.position.y() + other.height/2, 0);
		
		return Kore::max(Kore::abs(center.x() - centerOther.x()) - (width + other.width) / 2,
						 Kore::abs(center.y() - centerOther.y()) - (height + other.height) / 2);
	}
	
	// Return true iff there is an intersection with the other collider
	bool IntersectsWith(const BoxCollider& other) {
		if(position.x() + width < other.position.x() || position.x() > other.position.x() + other.width) return false;
		if(position.y() + height < other.position.y() || position.y() > other.position.y() + other.height) return false;
		return true;
	}
};

// A sphere is defined by a radius and a center
class SphereCollider {
public:
	Kore::vec3 center;
	float radius;
	
	// Collision normal is the normal vector pointing towards the other collider
	Kore::vec3 GetCollisionNormal(const SphereCollider& other) {
		Kore::vec3 n = other.center - center;
		n = n.normalize();
		return n;
	}
	
	Kore::vec3 GetCollisionNormal(const BoxCollider& other) {
		Kore::vec3 n = other.position - center;
		n = n.normalize();
		return n;
	}
	
	// The penetration depth with another sphere
	float PenetrationDepth(const SphereCollider& other) {
		return other.radius + radius - (other.center - center).getLength();
	}
	
	// Return true iff there is an intersection with the other sphere collider
	bool IntersectsWith(const SphereCollider& other) {
		return PenetrationDepth(other) > 0.0f;
	}
	
	// The penetration depth with another box
	float PenetrationDepth(const BoxCollider& other) {
		Kore::vec3 otherCenter = Kore::vec3(other.position.x() + tileWidth/2, other.position.y() + tileHeight/2, 0);
		return (otherCenter - center).getLength();
	}
	
	// Return true iff there is an intersection with the other box collider
	bool IntersectsWith(const BoxCollider& other) {
		float distance = PenetrationDepth(other);
		return distance < 2 * radius;
	}
};
