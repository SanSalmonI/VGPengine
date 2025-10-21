#pragma once

#include "Color.h"

namespace IExeEngine
{
	namespace Graphics
	{
		struct DirectionalLight
		{
			Color ambient = Colors::White; // Overall light color
			Color diffuse = Colors::White; // Base color
			Color specular = Colors::White; // Highlight color
			Math::Vector3 direction = Math::Vector3::ZAxis; // Pointing down the Z axis
			float padding = 0.0f; // Padding to make the structure 16 byte aligned
		};
	}
}