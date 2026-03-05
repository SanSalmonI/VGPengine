#include "Precompiled.h"
#include "RigidBody.h"
#include "CollisionShape.h"
#include "PhysicsWorld.h"

using namespace IExeEngine;
using namespace IExeEngine::Physics;

RigidBody::~RigidBody() {

	ASSERT(mRigidBody == nullptr, "RigidBody: Rigid body not terminated before destruction");
     }
void RigidBody::Initialize(Graphics::Transform& graphicsTransform, CollisionShape& shape, float mass)
{
	mGraphicsTransform = &graphicsTransform;
	mMass = mass;

	btVector3 localInertia = btVector3();
	shape.mCollisionShape->calculateLocalInertia(mMass, localInertia);
	mMotionState = new btDefaultMotionState(ConverTobtTransform(graphicsTransform));
	mRigidBody = new btRigidBody(mMass, mMotionState, shape.mCollisionShape);
	PhysicsWorld::Get()->Register(this);
			
}
void RigidBody::Terminate() {
	
	PhysicsWorld::Get()->Unregister(this);
	SafeDelete(mRigidBody);
	SafeDelete(mMotionState);

}
void RigidBody::SetPosition(const Math::Vector3& position) {

	mRigidBody->activate();
	mGraphicsTransform->position = position;
	mRigidBody->setWorldTransform(ConverTobtTransform(*mGraphicsTransform));

}
void RigidBody::SetVelocity(const Math::Vector3& velocity) {

	mRigidBody->activate();
	mRigidBody->setLinearVelocity(ToBtVector3(velocity));

}
const Math::Vector3& RigidBody::GetVelocity() const
{

	return ToVector3(mRigidBody->getLinearVelocity());
}

bool RigidBody::IsDynamic() const
{
	return mMass > 0.0f;
}

void RigidBody::SyncWithGraphics()
{
	btTransform transform = mRigidBody->getWorldTransform();
	mGraphicsTransform->position = ToVector3(transform.getOrigin());
	mGraphicsTransform->rotation = ToMathQuaternion(transform.getRotation());
}

btRigidBody* RigidBody::GetRigidBody()
{
	return mRigidBody;
}
