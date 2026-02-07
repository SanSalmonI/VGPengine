#pragma once
// framework headers

#include <Math/Inc/DWMath.h>
#include <Core/Inc/Core.h>
#include <Graphics/Inc/Graphics.h>

//Bullet headers
#include <Bullet/btBulletCollisionCommon.h>
#include <Bullet/btBulletDynamicsCommon.h>

//helper functions
template<class T>
inline void SafeDelete(T*& ptr)
{
	if (ptr)
	{
		delete ptr;
		ptr = nullptr;
	}
}

inline btVector3 ToBtVector3(const Math::Vector3& v)
{
	return btVector3(v.x, v.y, v.z);
}
inline Math::Vector3 ToMathVector3(const btVector3& v)
{
	return {
		static_cast<float>(v.getX()),
		static_cast<float>(v.getY()),
		static_cast<float>(v.getZ())
	};
}
inline btQuaternion ToBtQuaternion(const Math::Quaternion& q)
{
	return btQuaternion(q.x, q.y, q.z, q.w);
}
inline Math::Quaternion ToMathQuaternion(const btQuaternion& q)
{
	return {
		static_cast<float>(q.x()),
		static_cast<float>(q.y()),
		static_cast<float>(q.z()),
		static_cast<float>(q.w())
	};
}
inline Graphics::Color ToColor(const btVector3& v)
{
	return Graphics::Color{
		static_cast<float>(v.getX()),
		static_cast<float>(v.getY()),
		static_cast<float>(v.getZ()),
		1.0f
	};
}
inline btTransform ConverTobtTransform(const Graphics::Transform& t)
{
	return btTransform(ToBtQuaternion(t.rotation), ToBtVector3(t.position));
}