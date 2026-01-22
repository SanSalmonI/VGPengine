#pragma once

#include "ModelManager.h"

namespace IExeEngine::Graphics::AnimationUtil
{
	//defining a vector of bone matrices to use for skeleton calculations
	using BoneTransforms = std::vector<Math::Matrix4>;

	void ComouteBoneTransorms(ModelId, BoneTransforms& boneTransforms);
	//to be called after ComputeBoneTransform, draws the skeleton using SimpleDraw
	void DrawSkeleton(ModelId modelId, const BoneTransforms& boneTransofms);
	//to ve calles to apply bone offsets for skinning data
	void ApplyBoneOffsets(ModelId modelId, BoneTransforms& boneTransforms);

}
