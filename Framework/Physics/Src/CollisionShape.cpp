#include "CollisionShape.h"
#include "Common.h"

using namespace IExeEngine;

using namespace IExeEngine::Physics;


     CollisionShape::~CollisionShape(){
		 ASSERT(mCollisionShape == nullptr, "CollisionShape: Collision shape not terminated before destruction");
     }
void CollisionShape::InitializeEmpty(){
	ASSERT(mCollisionShape == nullptr, "CollisionShape: Collision shape EMPTY already initialized");
	mCollisionShape = new btEmptyShape();
}
void CollisionShape::InitializeSphere(float radius){
	ASSERT(mCollisionShape == nullptr, "CollisionShape: Collision shape Sphere already initialized");
	mCollisionShape = new btSphereShape(radius);
}
void CollisionShape::InitializeCapsule(float radius, float height){
	ASSERT(mCollisionShape == nullptr, "CollisionShape: Collision shape Capsule already initialized");
	mCollisionShape = new btCapsuleShape(radius, height);
}
void CollisionShape::InitializeBox(const Math::Vector3& halfExtents){
	ASSERT(mCollisionShape == nullptr, "CollisionShape: Collision shape Box already initialized");
	mCollisionShape = new btBoxShape(ToBtVector3(halfExtents));
}
void CollisionShape::InitializeHull(const Math::Vector3& halfExtents, const Math::Vector3& origin){
	ASSERT(mCollisionShape == nullptr, "CollisionShape: Collision shape Hull already initialized");
	btConvexHullShape* hullShape = new btConvexHullShape();
	std::vector<Math::Vector3> points =
	{
		{-halfExtents.x + origin.x, -halfExtents.y + origin.y, -halfExtents.z + origin.z},
		{-halfExtents.x + origin.x,  halfExtents.y + origin.y, -halfExtents.z + origin.z},
		{ halfExtents.x + origin.x,  halfExtents.y + origin.y, -halfExtents.z + origin.z},
		{ halfExtents.x + origin.x, -halfExtents.y + origin.y, -halfExtents.z + origin.z},
		{-halfExtents.x + origin.x, -halfExtents.y + origin.y, halfExtents.z + origin.z},
		{-halfExtents.x + origin.x, halfExtents.y + origin.y, halfExtents.z + origin.z},
		{ halfExtents.x + origin.x, halfExtents.y + origin.y, halfExtents.z + origin.z},
		{ halfExtents.x + origin.x, -halfExtents.y + origin.y, halfExtents.z + origin.z},


	};

	mCollisionShape = hullShape;

}
void CollisionShape::Terminate(){

}