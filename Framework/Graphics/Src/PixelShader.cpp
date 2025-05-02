#include "Precompiled.h"
#include "PixelShader.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;

void PixelShader::Initialize(const std::filesystem::path& shaderPath)
{
}

void PixelShader::Terminate()
{
	SafeRelease(mPixelShader);
}

void PixelShader::Bind()
{
	auto context = GraphicsSystem::Get()->GetContext();
}
