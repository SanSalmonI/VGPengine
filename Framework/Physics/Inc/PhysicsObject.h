#pragma once

//object that is connected to physics world. must be inherited to register with world

namespace IExeEngine::Physics
{
	
	class PhysicsObject
	{
	public:
		PhysicsObject() = default;
		virtual ~PhysicsObject() = default;

	protected:
		friend class PhysicsWorld;

		//abstract function 
		virtual void SyncWithGraphics() = 0;
		virtual btRigidBody* GetRigidBody() { return nullptr; }
		virtual btSoftBody* GetSoftBody() { return nullptr; }
	};
}