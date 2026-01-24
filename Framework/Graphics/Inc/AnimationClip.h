#pragma once

#include "Animation.h"

namespace IExeEngine::Graphics
{
	struct AnimationClip
	{
		std::string name;
		float tickDuration = 0.0f; // in seconds
		float ticksPerSecond = .0f; // default to 24 fps
		std::vector<std::unique_ptr<Animation>> boneAnimations; // one animation per bone
	};
}