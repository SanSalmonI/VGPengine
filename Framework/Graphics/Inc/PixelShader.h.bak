#pragma once

namespace IExeEngine::Graphics
{
	class PixelShader final
	{
	public:
		void Initialize(const std::filesystem::path& shaderPath, const char* entryPoint = "PS");
		void Terminate();
		void Bind();

	private:
		ID3D11PixelShader* mPixelShader = nullptr;
	};
}