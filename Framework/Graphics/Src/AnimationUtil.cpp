#include "Precompiled.h"
#include "AnimationUtil.h"

#include "Color.h"
#include "SimpleDraw.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;

//enpty namespace for global functions isolated to the cpp file

namespace 
{
	void conputeBoneTransformsRecursive(const Bone* bone, AnimationUtil::BoneTransforms& boneTransforms) {
		if (bone != nullptr)
		{
			//set the bone transform to the array of matrices
			boneTransforms[bone->index] = bone->toParentTransform;
			//if there is a parent, apply the parent's transform as well
			if (bone->parent != nullptr)
			{
				boneTransforms[bone->index] = boneTransforms[bone->index] * boneTransforms[bone->parent->index];
			}
			for (const Bone* child : bone->children)
			{
				conputeBoneTransformsRecursive(child, boneTransforms);
			}
			return;
		}
	}
}

void AnimationUtil::ComouteBoneTransorms(ModelId modelId, BoneTransforms& boneTransforms)
{
	const Model* model = ModelManager::Get()->GetModel(modelId);
	if (model != nullptr && model->skeleton != nullptr)
	{
		//resize the bone transforms to match the number of bones in the skeleton
		boneTransforms.resize(model->skeleton->bones.size());
		conputeBoneTransformsRecursive(model->skeleton->root, boneTransforms);

	}
}
	 
void AnimationUtil::DrawSkeleton(ModelId modelId, const BoneTransforms& boneTransofms)
{
	const Model* model = ModelManager::Get()->GetModel(modelId);
	if (model != nullptr && model->skeleton != nullptr)
	{
		//iterate through each bone in the skeleton
		for (const auto& bone : model->skeleton->bones)
		{
			if (bone->parent != nullptr)
			{
				const Math::Vector3 bonePos = Math::GetTranslation(boneTransofms[bone->index]);
				const Math::Vector3 parentPos = Math::GetTranslation(boneTransofms[bone->parentIndex]);
				SimpleDraw::AddLine(parentPos, bonePos, Colors::Honeydew);
				SimpleDraw::AddSphere(10, 10, 0.03f, Colors::White, bonePos);
			}
		}
	}
}

