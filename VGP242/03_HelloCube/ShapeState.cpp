#include "ShapeState.h"

using namespace IExeEngine;
using namespace IExeEngine::Math;
using namespace IExeEngine::Graphics;

void ShapeState::Initialize()
{
	// Creates a shape out of the vertices
	CreateShape();
	mMeshbuffer.Initialize(mVertices.data(), sizeof(VertexPC), mVertices.size());

	std::filesystem::path shaderFilePath = L"../../Assets/Shaders/DoColor.fx";

	mVertexShader.Initialize<VertexPC>(shaderFilePath);

	mPixelShader.Initialize(shaderFilePath);
}

void ShapeState::Terminate()
{
	mVertices.clear();
	mPixelShader.Terminate();
	mVertexShader.Terminate();
	mMeshbuffer.Terminate();
}

void ShapeState::Update(float deltaTime)
{

}

void ShapeState::Render()
{
	// Prepare GPU
	mVertexShader.Bind();
	mPixelShader.Bind();

	// Draw
	mMeshbuffer.Render();
}

void ShapeState::CreateShape()
{
	mVertices.push_back({ { -0.5f,0.0f,0.0f }, Colors::Red });
	mVertices.push_back({ {  0.0f,0.75f,0.0f }, Colors::Yellow });
	mVertices.push_back({ {  0.5f,0.0f,0.0f }, Colors::Red });

	mVertices.push_back({ { -0.5f,0.0f,0.0f }, Colors::Red });
	mVertices.push_back({ {  0.5f,0.0f,0.0f }, Colors::Yellow });
	mVertices.push_back({ {  0.0f, -0.75f,0.0f }, Colors::Red });
}