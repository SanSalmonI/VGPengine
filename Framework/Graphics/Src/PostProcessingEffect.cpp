#include "Precompiled.h"
#include "PostProcessingEffect.h"

#include "RenderObject.h"
#include "Texture.h"
#include "VertexTypes.h"
#include "GraphicsSystem.h"

using namespace IExeEngine;	
using namespace IExeEngine::Graphics;

namespace 
{
	const char* gMofeNames[] =
	{
		"None",
		"MonoChrome",
		"Invert",
		"Mirror",
		"Blur",
		"Combine2",
		"MotionBlur",
		"chromeAberration",
		"Wave"
	};
}

void PostProcessingEffect::Initialize(const std::filesystem::path& filepath) 
{
	mVertexShader.Initialize<VertexPX>(filepath);
	mPixelShader.Initialize(filepath);
	mSampler.Initialize(Sampler::Filter::Point, Sampler::AddressMode::Wrap);
	mPostProcessBuffer.Initialize();
}
void PostProcessingEffect::Terminate()
{
	mSampler.Terminate();
	mPixelShader.Terminate();
	mVertexShader.Terminate();
	mPostProcessBuffer.Terminate();
}

void  PostProcessingEffect::Begin()
{
	mVertexShader.Bind();
	mPixelShader.Bind();
	mSampler.BindPS(0);

	for(uint32_t i = 0; i < mTextures.size(); ++i)
	{
		if (mTextures[i] != nullptr)
		{
			mTextures[i]->BindPS(i);
		}
	}
	PostProcessingData data;
	data.mode = static_cast<int>(mMode);

	switch (mMode)
	{
		case Mode::None:
		case Mode::Mirror:
		{
			data.param0 = mMirrorScaleX;
			data.param1 = mMirrorScaleY;
		}
			break;
		case Mode::Invert:
			break;
		case Mode::MotionBlur:
		{
			GraphicsSystem* gs = GraphicsSystem::Get();
			const float screenWidth = gs->GetBackBufferWidth();
			const float screenHeight = gs->GetBackBufferHeight();
			data.param0 = mBlurrStrength / screenWidth;
			data.param1 = mBlurrStrength / screenHeight;
		}
			break;
		case Mode::Combine2:
		{
			data.param0 = mCombine2Alpha;
		}
		break;
		case Mode::chromeAberration:
		{
			data.param0 = mAberrationVale;
			data.param1 = mAberrationVale;

		}
			break;
		case Mode::Wave:
		{
			data.param0 = mWaveLength;
			data.param1 = mWaveAmplitude;
		}
		break;
		default:
			
			break;
	}

	mPostProcessBuffer.Update(data);
	mPostProcessBuffer.BindPS(0);
}
void PostProcessingEffect::End()
{
	for(uint32_t i = 0; i < mTextures.size(); ++i)
	{
		Texture::UnbindPS(i);
	}
}

void PostProcessingEffect::Render(const RenderObject& renderObject)
{
	renderObject.meshBuffer.Render();
}
void PostProcessingEffect::SetTexture(const Texture* texture, uint32_t slot)
{
	ASSERT(slot < mTextures.size(), "PostProcessingEffect::SetTexture(): invalid slot index ");
	mTextures[slot] = texture;
}

void PostProcessingEffect::SetMode(Mode mode)
{
	mMode = mode;
}

void PostProcessingEffect::DebugUI()
{
	if(ImGui::CollapsingHeader("PostProcessingEffect", ImGuiTreeNodeFlags_DefaultOpen))
	{
		int currentMode = static_cast<int>(mMode);
		if(ImGui::Combo("Mode", &currentMode, gMofeNames, std::size(gMofeNames)))
		{
			mMode = static_cast<Mode>(currentMode);
		}
		if(mMode == Mode::Mirror)
		{
			ImGui::DragFloat("MirrorScaleX", &mMirrorScaleX, 0.01f, -1.0f, 1.0f);
			ImGui::DragFloat("MirrorScaleY", &mMirrorScaleY, 0.01f, -1.0f, 1.0f);
		}
		else if (mMode == Mode::Blur || mMode == Mode::MotionBlur)
		{
			ImGui::DragFloat("BlurStrength", &mBlurrStrength, 0.1f, 0.0f, 100.0f);
		}
		else if (mMode == Mode::Combine2)
		{
			ImGui::DragFloat("CombineAlpha", &mCombine2Alpha, 0.01f, 0.0f, 1.0f);
		}
		else if (mMode == Mode::chromeAberration)
		{
			ImGui::DragFloat("AberrationVale", &mAberrationVale, 0.0001f, 0.0f, 0.1f);
		}
		else if (mMode == Mode::Wave)
		{
			ImGui::DragFloat("WaveLength", &mWaveLength, 0.001f, 0.0f, 0.5f);
			ImGui::DragFloat("WaveAmplitude", &mWaveAmplitude, 1.0f, 0.0f, 1000.0f);
		}
	}
}