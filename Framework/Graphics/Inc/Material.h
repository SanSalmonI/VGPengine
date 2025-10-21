#pragma once
#include "Color.h"

namespace IExeEngine::Graphics
{
	struct Material
	{
		Color emissive = Colors::Black; // color where light is abscent
		Color ambient = Colors::White;  // color when light is low
		Color diffuse = Colors::White;  // base light color
		Color specular = Colors::White; // color for shiny area
		float shininess = 10.0f;        // intensity of the light highlights
		float padding[3] = { 0.0f };    // padding to make the structure 16 byte aligned
	};
}