#pragma once

#include <IExeEngine/Inc/IExeEngine.h>

class ShapeState : public IExeEngine::AppState
{
public:
	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Render() override;

protected:
	virtual void CreateShape();

	struct Vertex
	{
		IExeEngine::Math::Vector3 position;
		IExeEngine::Graphics::Color color;
	};
	using Vertices = std::vector<Vertex>;
	Vertices mVertices;

	ID3D11Buffer* mVertexBuffer = nullptr;
	ID3D11VertexShader* mVertexShader = nullptr;
	ID3D11InputLayout* mInputLayout = nullptr;
	ID3D11PixelShader* mPixelShader = nullptr;
};

class Bunny : public ShapeState
{
public:
	void Update(float deltaTime) override;
protected:
	void CreateShape() override;
};

class Butterfly : public ShapeState
{
public:
	void Update(float deltaTime) override;
protected:
	void CreateShape() override;
};

class Fish : public ShapeState
{
public:
	void Update(float deltaTime) override;
protected:
	void CreateShape() override;
};