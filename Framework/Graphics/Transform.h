#pragma once

namespace IExeEngine::Graphics
{
	struct Transform
	{
		Math::Vector3 position = Math::Vector2::Zero;
		Math::Quaternion rotation = Math::Quaternion::Identity;
		Math::Vector3 scale = Math::Vector3::One;

		Math::Matrix4 ToMatrix() const
		// s x r x p
		{
			return 
				Math::Matrix4::Scaling(scale) *
				Math::Matrix4::MatrixRotationQuaternion(rotation) *
				Math::Matrix4::Translation(position);

		}
	};
}
