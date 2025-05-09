#include "ShapeState.h"

using namespace IExeEngine;
using namespace IExeEngine::Math;
using namespace IExeEngine::Graphics;

void ShapeState::Initialize()
{
	// Creates a shape out of the vertices
	CreateShape();

	auto device = GraphicsSystem::Get()->GetDevice();

	// Need to create a buffer to store the vertices
	// STORES DATA FOR THE OBJECT
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = static_cast<UINT>(mVertices.size()) * sizeof(Vertex);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = mVertices.data();

	HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &mVertexBuffer);
	ASSERT(SUCCEEDED(hr), "Failed to create Vertex Buffer");
	//====================================================================================================

	// BIND TO FUNCTION IN SPECIFIED SHADER FILE
	std::filesystem::path shaderFilePath = L"../../Assets/Shaders/DoColor.fx";

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3DCompileFromFile(
		shaderFilePath.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VS", "vs_5_0",
		shaderFlags, 0,
		&shaderBlob,
		&errorBlob);

	if (errorBlob != nullptr && errorBlob->GetBufferPointer() != nullptr)
	{
		LOG("%s", static_cast<const char*>(errorBlob->GetBufferPointer()));
	}
	ASSERT(SUCCEEDED(hr), "Failed to create Vertex Shader");

	hr = device->CreateVertexShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		nullptr,
		&mVertexShader);
	ASSERT(SUCCEEDED(hr), "Failed to create Vertex Shader");
	//======================================================================================================

	// STATE WHAT THE VERTEX VARIABLES ARE
	std::vector<D3D11_INPUT_ELEMENT_DESC> vertexLayout;
	vertexLayout.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT });
	vertexLayout.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT });

	hr = device->CreateInputLayout(
		vertexLayout.data(),
		static_cast<UINT>(vertexLayout.size()),
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		&mInputLayout);
	ASSERT(SUCCEEDED(hr), "Failed to create Input Layout");
	SafeRelease(shaderBlob);
	SafeRelease(errorBlob);
	//======================================================================================================

	// BIND TO PIXEL FUNCTION IN SPECIFIED SHADER FILE
	hr = D3DCompileFromFile(
		shaderFilePath.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PS", "ps_5_0",
		shaderFlags, 0,
		&shaderBlob,
		&errorBlob);

	if (errorBlob != nullptr && errorBlob->GetBufferPointer() != nullptr)
	{
		LOG("%s", static_cast<const char*>(errorBlob->GetBufferPointer()));
	}
	ASSERT(SUCCEEDED(hr), "Failed to create Pixel Shader");

	hr = device->CreatePixelShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		nullptr,
		&mPixelShader);
	ASSERT(SUCCEEDED(hr), "Failed to create Pixel Shader");
	SafeRelease(shaderBlob);
	SafeRelease(errorBlob);
}

void ShapeState::Terminate()
{
	mVertices.clear();
	SafeRelease(mPixelShader);
	SafeRelease(mInputLayout);
	SafeRelease(mVertexShader);
	SafeRelease(mVertexBuffer);
}

void ShapeState::Render()
{
	auto context = GraphicsSystem::Get()->GetContext();
	// Bind buffers
	context->VSSetShader(mVertexShader, nullptr, 0);
	context->IASetInputLayout(mInputLayout);
	context->PSSetShader(mPixelShader, nullptr, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	context->Draw(static_cast<UINT>(mVertices.size()), 0);
}

void ShapeState::Update(float deltaTime)
{
	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::UP))
	{
		IExeEngine::MainApp().ChangeState("Bunny");
	}

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::LEFT))
	{
		IExeEngine::MainApp().ChangeState("Butterfly");
	}

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::RIGHT))
	{
		IExeEngine::MainApp().ChangeState("Fish");
	}
}

void ShapeState::CreateShape()
{
	mVertices.push_back({ { 0.20f,  -0.80f, 0.0f }, Colors::Yellow });
	mVertices.push_back({ { 0.02f,  0.05f, 0.0f }, Colors::GreenYellow });
	mVertices.push_back({ { 0.50f, -0.27f, 0.0f }, Colors::Yellow });


}

void Bunny::Update(float deltaTime)
{
	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::DOWN))
	{
		IExeEngine::MainApp().ChangeState("ShapeState");
	}

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::LEFT))
	{
		IExeEngine::MainApp().ChangeState("Butterfly");
	}

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::RIGHT))
	{
		IExeEngine::MainApp().ChangeState("Fish");
	}
}

void Bunny::CreateShape()
{

	// 1) Ears
	mVertices.push_back({ { 0.00f,  0.30f, 0.0f }, Colors::SaddleBrown });
	mVertices.push_back({ { 0.30f,  0.70f, 0.0f }, Colors::SandyBrown });
	mVertices.push_back({ { 0.10f,  0.15f, 0.0f }, Colors::Brown });

	// 2) Head (upper)
	mVertices.push_back({ { -0.12f,  0.05f, 0.0f }, Colors::RosyBrown });
	mVertices.push_back({ {  0.00f,  0.25f, 0.0f }, Colors::Brown });
	mVertices.push_back({ {  0.11f,  0.17f, 0.0f }, Colors::Brown });
	//    Head (lower)
	mVertices.push_back({ { -0.12f,  0.05f, 0.0f }, Colors::RosyBrown });
	mVertices.push_back({ {  0.11f,  0.17f, 0.0f }, Colors::Brown });
	mVertices.push_back({ {  0.05f, -0.10f, 0.0f }, Colors::SaddleBrown });

	// 3) Back ear
	mVertices.push_back({ { 0.00f,  0.40f, 0.0f }, Colors::SaddleBrown });
	mVertices.push_back({ { 0.20f,  0.75f, 0.0f }, Colors::Firebrick });
	mVertices.push_back({ { 0.10f,  0.25f, 0.0f }, Colors::SaddleBrown });

	// Foot
	mVertices.push_back({ { 0.03f, -0.60f, 0.0f }, Colors::SandyBrown });
	mVertices.push_back({ { 0.15f, -0.45f, 0.0f }, Colors::Brown });
	mVertices.push_back({ { 0.20f, -0.60f, 0.0f }, Colors::SaddleBrown });

	// Back legs
	mVertices.push_back({ { 0.10f, -0.60f, 0.0f }, Colors:: SaddleBrown });
	mVertices.push_back({ { 0.15f, -0.10f, 0.0f }, Colors::SandyBrown });
	mVertices.push_back({ { 0.40f, -0.10f, 0.0f }, Colors::SandyBrown });

	mVertices.push_back({ { 0.10f, -0.60f, 0.0f }, Colors::SaddleBrown });
	mVertices.push_back({ { 0.40f, -0.10f, 0.0f }, Colors::SandyBrown });
	mVertices.push_back({ { 0.40f, -0.60f, 0.0f }, Colors::SaddleBrown });

	// 4) Body
	mVertices.push_back({ { -0.05f,  0.05f, 0.0f }, Colors::SaddleBrown });
	mVertices.push_back({ {  0.40f, -0.10f, 0.0f }, Colors::SandyBrown });
	mVertices.push_back({ {  0.25f, -0.55f, 0.0f }, Colors::SaddleBrown });
	// Front legs
	mVertices.push_back({ { -0.05f, -0.50f, 0.0f }, Colors::SandyBrown });
	mVertices.push_back({ {  0.00f, -0.10f, 0.0f }, Colors::SaddleBrown });
	mVertices.push_back({ {  0.15f, -0.10f, 0.0f }, Colors::Brown });
	// Chest
	mVertices.push_back({ {  0.00f,  0.05f, 0.0f }, Colors::SaddleBrown });
	mVertices.push_back({ {  0.25f, -0.55f, 0.0f }, Colors::SaddleBrown });
	mVertices.push_back({ { -0.05f, -0.30f, 0.0f }, Colors::Brown });

	//back front leg

	mVertices.push_back({ { -0.10f, -0.45f, 0.0f }, Colors::SandyBrown });
	mVertices.push_back({ {  0.00f, -0.10f, 0.0f }, Colors::SaddleBrown });
	mVertices.push_back({ {  0.15f, -0.10f, 0.0f }, Colors::SaddleBrown });
}

    



void Butterfly::Update(float deltaTime)
{
	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::DOWN))
	{
		IExeEngine::MainApp().ChangeState("ShapeState");
	}

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::UP))
	{
		IExeEngine::MainApp().ChangeState("Bunny");
	}

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::RIGHT))
	{
		IExeEngine::MainApp().ChangeState("Fish");
	}
}

void Butterfly::CreateShape()
{

	// 1) Upper Wing R
	mVertices.push_back({ { 0.10f,  0.05f, 0.0f }, Colors::Blue });
	mVertices.push_back({ { 0.60f,  0.60f, 0.0f }, Colors::LightCyan });
	mVertices.push_back({ { 0.40f, -0.15f, 0.0f }, Colors::White });

	mVertices.push_back({ { 0.02f,  0.05f, 0.0f }, Colors::Black });
	mVertices.push_back({ { 0.70f,  0.80f, 0.0f }, Colors::DimGray });
	mVertices.push_back({ { 0.40f, -0.30f, 0.0f }, Colors::DarkSlateBlue });

	//2) Lower Wing R
	mVertices.push_back({ { 0.22f,  -0.70f, 0.0f }, Colors::DarkCyan });
	mVertices.push_back({ { 0.05f,  0.05f, 0.0f }, Colors::Blue });
	mVertices.push_back({ { 0.45f, -0.25f, 0.0f }, Colors::White });

	mVertices.push_back({ { 0.20f,  -0.80f, 0.0f }, Colors::DimGray });
	mVertices.push_back({ { 0.02f,  0.05f, 0.0f }, Colors::Black });
	mVertices.push_back({ { 0.50f, -0.27f, 0.0f }, Colors::DarkSlateBlue });

// 1') UPPER WING L 
	mVertices.push_back({ { -0.60f,  0.60f, 0.0f }, Colors::LightCyan }); 
	mVertices.push_back({ { -0.10f,  0.05f, 0.0f }, Colors::Blue });
	mVertices.push_back({ { -0.40f, -0.15f, 0.0f }, Colors::White });


	mVertices.push_back({ { -0.70f,  0.80f, 0.0f }, Colors::DimGray }); 
	mVertices.push_back({ { -0.02f,  0.05f, 0.0f }, Colors::Black });
	mVertices.push_back({ { -0.40f, -0.30f, 0.0f }, Colors::DarkSlateBlue });


	// 2') LOWER WING L 

	mVertices.push_back({ { -0.45f, -0.25f, 0.0f }, Colors::White }); 
	mVertices.push_back({ { -0.05f,  0.05f, 0.0f }, Colors::Blue });
	mVertices.push_back({ { -0.22f, -0.70f, 0.0f }, Colors::DarkCyan });


	// 2'') LOWER WING L 
	mVertices.push_back({ { -0.50f, -0.27f, 0.0f }, Colors::DarkSlateBlue }); 
	mVertices.push_back({ { -0.02f,  0.05f, 0.0f }, Colors::Black });
	mVertices.push_back({ { -0.20f, -0.80f, 0.0f }, Colors::DimGray });


	//3) Upper body
	mVertices.push_back({ { -0.05f,  0.10f, 0.0f }, Colors::DarkKhaki });
	mVertices.push_back({ { 0.00f,  0.20f, 0.0f }, Colors::Brown });
	mVertices.push_back({ { 0.05f, 0.10f, 0.0f }, Colors::Brown });

	//3) lower body
	mVertices.push_back({ { -0.05f,  0.10f, 0.0f }, Colors::DarkKhaki });
	mVertices.push_back({ { 0.05f, 0.10f, 0.0f }, Colors::Brown });
	mVertices.push_back({ { 0.00f,  -0.40f, 0.0f }, Colors::Brown });
}

void Fish::Update(float deltaTime)
{
	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::DOWN))
	{
		IExeEngine::MainApp().ChangeState("ShapeState");
	}

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::LEFT))
	{
		IExeEngine::MainApp().ChangeState("Butterfly");
	}

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::UP))
	{
		IExeEngine::MainApp().ChangeState("Bunny");
	}
}

void Fish::CreateShape()
{
	//eye
	mVertices.push_back({ { 0.40f,  0.05f, 0.0f }, Colors::Wheat });
	mVertices.push_back({ { 0.45f,  0.00f, 0.0f }, Colors::Black });
	mVertices.push_back({ { 0.40f, -0.05f, 0.0f }, Colors::White });

	//fin
	mVertices.push_back({ { 0.15f,  -0.05f, 0.0f }, Colors::LightCyan });
	mVertices.push_back({ { 0.30f,  -0.15f, 0.0f }, Colors::RoyalBlue });
	mVertices.push_back({ { 0.20f, -0.25f, 0.0f }, Colors::Cyan });

	// 1) BODY  – large right-pointing triangle
	mVertices.push_back({ { 0.00f,  0.40f, 0.0f }, Colors::LightCyan }); 
	mVertices.push_back({ { 0.60f,  0.00f, 0.0f }, Colors::Blue }); 
	mVertices.push_back({ { 0.00f, -0.40f, 0.0f }, Colors::DarkBlue }); 

	// 2) TAIL  
	mVertices.push_back({ { -0.20f,  0.30f, 0.0f }, Colors::LightGreen }); 
	mVertices.push_back({ { 0.00f,  0.00f, 0.0f }, Colors::DarkBlue }); 
	mVertices.push_back({ { -0.20f, -0.30f, 0.0f }, Colors::DarkGreen }); 


	// 3) DORSAL FIN 
	mVertices.push_back({ { 0.05f,  0.35f, 0.0f }, Colors::Blue }); 
	mVertices.push_back({ { 0.10f,  0.60f, 0.0f }, Colors::SkyBlue }); 
	mVertices.push_back({ { 0.25f,  0.20f, 0.0f }, Colors::Blue }); 


	
	
}
