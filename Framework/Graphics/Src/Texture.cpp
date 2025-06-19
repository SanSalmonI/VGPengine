#include "Precompiled.h"
#include "Texture.h"

#include "GraphicsSystem.h"
#include <DirectXTK/Inc/WICTextureLoader.h>

using namespace IExeEngine;
using namespace IExeEngine::Graphics;

void Texture::UnbindPS(uint32_t slot)
{
    static ID3D11ShaderResourceView* dummy = nullptr;
    GraphicsSystem::Get()->GetContext()->HSSetShaderResources(slot, 1, &dummy);
}

Texture::~Texture()
{
    ASSERT(mShaderResourceView == nullptr, "Texture: Terminate must be called");
}

Texture::Texture(Texture&& rhs) noexcept
    : mShaderResourceView(rhs.mShaderResourceView)
{
    rhs.mShaderResourceView = nullptr;
}

Texture& Texture::operator=(Texture&& rhs) noexcept
{
    mShaderResourceView = rhs.mShaderResourceView;
    rhs.mShaderResourceView = nullptr;
    return *this;
}

void Texture::Initialize(const std::filesystem::path& fileName)
{
    ASSERT(std::filesystem::exists(fileName),
        "Texture: File not found: %s", fileName.string().c_str());

    auto device = GraphicsSystem::Get()->GetDevice();
    auto context = GraphicsSystem::Get()->GetContext();

    HRESULT hr = DirectX::CreateWICTextureFromFile(
        device,
        context,
        fileName.wstring().c_str(),   // pass a `const wchar_t*`
        nullptr,
        &mShaderResourceView);

    ASSERT(SUCCEEDED(hr),
        "Texture: CreateWICTextureFromFile failed for %s (HR=0x%08X)",
        fileName.string().c_str(), hr);
}


void Texture::Terminate()
{
    SafeRelease(mShaderResourceView);
}

void Texture::BindVS(uint32_t slot) const
{
    auto context = GraphicsSystem::Get()->GetContext();
    context->VSSetShaderResources(slot, 1, &mShaderResourceView);
}

void Texture::BindPS(uint32_t slot) const
{
    auto context = GraphicsSystem::Get()->GetContext();
    context->PSSetShaderResources(slot, 1, &mShaderResourceView);
}

void* Texture::GetRawData() const
{
    return mShaderResourceView;
}
