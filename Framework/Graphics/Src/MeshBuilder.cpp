#include "Precompiled.h"
#include "MeshBuilder.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;

namespace
{
	Color GetNextColor(int& index)
	{
		constexpr Color colorTable[] =
		{
			Colors::Red,
			Colors::Green,
			Colors::Blue,
			Colors::Yellow,
			Colors::Cyan,
			Colors::Magenta,
			Colors::White,
			Colors::Pink
		};

		index = (index + 1) % std::size(colorTable);
		return colorTable[index];
	}

	void CreateCubeIndices(std::vector<uint32_t>& indices)
	{
		indices = {
			//front
			0, 1, 2,
			0, 2, 3,
			//back
			7, 5, 4,
			7, 6, 5,
			//right
			3, 2, 6,
			3, 6, 7,
			//left
			4, 5, 1,
			4, 1, 0,
			//top
			1, 5, 6,
			1, 6, 2,
			//bottom
			0, 3, 7,
			0, 7, 4
		};
	}

	void CreatePlaneIndices(std::vector<uint32_t>& indices, int numRows, int numColumns)
	{
		for (int r = 0; r < numRows; ++r)
		{
			for (int c = 0; c < numColumns; ++c)
			{
				int i = (r * (numColumns + 1)) + c;

				//triangle 0
				indices.push_back(i);
				indices.push_back(i + numColumns + 1);
				indices.push_back(i + numColumns + 2);

				//triangle 1
				indices.push_back(i);
				indices.push_back(i + numColumns + 2);
				indices.push_back(i + 1);
			}
		}
	}

	void CreateCapIndices(std::vector<uint32_t>& indices, int slices, int topIndex, int bottomIndex)
	{
		for (int s = 0; s < slices; ++s)
		{
			//bottom triangle
			indices.push_back(bottomIndex);
			indices.push_back(s);
			indices.push_back(s + 1);

			//top triangle
			int topRowIndex = topIndex - slices - 1 + s;
			indices.push_back(topIndex);
			indices.push_back(topRowIndex + 1);
			indices.push_back(topRowIndex);
		}
	}
}

MeshPC MeshBuilder::CreateCubePC(float size, const Color& color)
{
	MeshPC mesh;


	const float hs = 0.5f;
	mesh.vertices.push_back({ { -hs, -hs, -hs }, color });
	mesh.vertices.push_back({ { -hs,  hs, -hs }, color });
	mesh.vertices.push_back({ {  hs,  hs, -hs }, color });
	mesh.vertices.push_back({ {  hs, -hs, -hs }, color });

	//back
	mesh.vertices.push_back({ { -hs, -hs, hs }, color });
	mesh.vertices.push_back({ { -hs,  hs, hs }, color });
	mesh.vertices.push_back({ {  hs,  hs, hs }, color });
	mesh.vertices.push_back({ {  hs, -hs, hs }, color });

	CreateCubeIndices(mesh.indices);

	return mesh;
}

MeshPC MeshBuilder::CreateCubePC(float size)
{
	MeshPC mesh;

	int index = rand() % 100;

	const float hs = 0.5f;
	//front
	mesh.vertices.push_back({ { -hs, -hs, -hs }, GetNextColor(index) });
	mesh.vertices.push_back({ { -hs,  hs, -hs }, GetNextColor(index) });
	mesh.vertices.push_back({ {  hs,  hs, -hs }, GetNextColor(index) });
	mesh.vertices.push_back({ {  hs, -hs, -hs }, GetNextColor(index) });

	//back
	mesh.vertices.push_back({ { -hs, -hs, hs }, GetNextColor(index) });
	mesh.vertices.push_back({ { -hs,  hs, hs }, GetNextColor(index) });
	mesh.vertices.push_back({ {  hs,  hs, hs }, GetNextColor(index) });
	mesh.vertices.push_back({ {  hs, -hs, hs }, GetNextColor(index) });

	CreateCubeIndices(mesh.indices);

	return mesh;
}

//MeshPX MeshBuilder::CreateCubePX(float size)
//{
//	MeshPX mesh;
//	const float hs = 0.5f;
//	mesh.vertices.push_back({ { -hs, -hs, -hs }, { 0.0f, 0.0f } });
//	mesh.vertices.push_back({ { -hs,  hs, -hs }, { 0.0f, 1.0f } });
//	mesh.vertices.push_back({ {  hs,  hs, -hs }, { 1.0f, 1.0f } });
//	mesh.vertices.push_back({ {  hs, -hs, -hs }, { 1.0f, 0.0f } });
//	//back
//	mesh.vertices.push_back({ { -hs, -hs, hs }, { 0.0f, 0.0f } });
//	mesh.vertices.push_back({ { -hs,  hs, hs }, { 0.0f, 1.0f } });
//	mesh.vertices.push_back({ {  hs,  hs, hs }, { 1.0f, 1.0f } });
//	mesh.vertices.push_back({ {  hs, -hs, hs }, { 1.0f, 0.0f } });
//	CreateCubeIndices(mesh.indices);
//	return mesh;
//
//}

MeshPC MeshBuilder::CreatePyramidPC(float size)
{
	MeshPC mesh;

	int index = rand() % 100;
	const float hs = 0.5f;
	// add all the vertices for the pyramid
	//front face
	mesh.vertices.push_back({ { -hs, -hs, -hs }, GetNextColor(index) });
	mesh.vertices.push_back({ {   0,  hs,   0 }, GetNextColor(index) });// top
	mesh.vertices.push_back({ {  hs, -hs, -hs }, GetNextColor(index) });

	//back
	mesh.vertices.push_back({ { -hs, -hs, hs }, GetNextColor(index) });
	mesh.vertices.push_back({ {  hs, -hs, hs }, GetNextColor(index) });

	// add all the indices for the pyramid
	mesh.indices = {
		//front
		0, 1, 2,
		//back
		3, 4, 1,
		//right
		2, 1, 4,
		//left
		0, 3, 1,
		//bottom
		0, 2, 4,
		0, 4, 3
	};

	return mesh;
}

MeshPC MeshBuilder::CreateRectanglePC(float width, float height, float depth)
{
	MeshPC mesh;

	int index = rand() % 100;
	const float hw = width * 0.5f;
	const float hh = height * 0.5f;
	const float hd = depth * 0.5f;

	// add all the vertices for the rectangle
	//front face
	mesh.vertices.push_back({ { -hw, -hh, -hd }, GetNextColor(index) });
	mesh.vertices.push_back({ { -hw,  hh, -hd }, GetNextColor(index) });
	mesh.vertices.push_back({ {  hw,  hh, -hd }, GetNextColor(index) });
	mesh.vertices.push_back({ {  hw, -hh, -hd }, GetNextColor(index) });

	//back face
	mesh.vertices.push_back({ { -hw, -hh,  hd }, GetNextColor(index) });
	mesh.vertices.push_back({ { -hw,  hh,  hd }, GetNextColor(index) });
	mesh.vertices.push_back({ {  hw,  hh,  hd }, GetNextColor(index) });
	mesh.vertices.push_back({ {  hw, -hh,  hd }, GetNextColor(index) });

	// add all the indices for the rectangle
	CreateCubeIndices(mesh.indices);

	return mesh;
}

MeshPC MeshBuilder::CreatePlanePC(int numRows, int numCols, float spacing, bool horizontal)
{
	MeshPC mesh;
	int index = rand() % 100;
	const float hpw = static_cast<float>(numCols) * spacing * 0.5f;
	const float hph = static_cast<float>(numRows) * spacing * 0.5f;

	float w = -hpw;
	float h = -hph;
	for (int r = 0; r <= numRows; ++r)
	{
		for (int c = 0; c <= numCols; ++c)
		{
			Math::Vector3 pos = (horizontal) ? Math::Vector3(w, 0.0f, h) : Math::Vector3(w, h, 0.0f);
			mesh.vertices.push_back({ pos, GetNextColor(index) });
			w += spacing;
		}
		w = -hpw;
		h += spacing;

	}

	CreatePlaneIndices(mesh.indices, numRows, numCols);

	return mesh;
}

MeshPX MeshBuilder::CreatePlanePX(int numRows, int numCols, float spacing, bool horizontal)
{
	MeshPX mesh;

	const float hpw = static_cast<float>(numCols) * spacing * 0.5f;
	const float hph = static_cast<float>(numRows) * spacing * 0.5f;
	const float uInc = 1.0f / static_cast<float>(numCols);
	const float vInc = 1.0f / static_cast<float>(numRows);

	float w = -hpw;
	float h = -hph;
	float u = 0.0f;
	float v = 1.0f;
	for (int r = 0; r <= numRows; ++r)
	{
		for (int c = 0; c <= numCols; ++c)
		{
			Math::Vector3 pos = (horizontal) ? Math::Vector3(w, 0.0f, h) : Math::Vector3(w, h, 0.0f);
			mesh.vertices.push_back({ pos, {u, v } });
			w += spacing;
			u += uInc;
		}
		w = -hpw;
		h += spacing;
		u = 0.0f;
		v -= vInc;

	}

	CreatePlaneIndices(mesh.indices, numRows, numCols);

	return mesh;
}

Mesh MeshBuilder::CreatePlane(int numRows, int numCols, float spacing, bool horizontal)
{
	Mesh mesh;

	const float hpw = static_cast<float>(numCols) * spacing * 0.5f;
	const float hph = static_cast<float>(numRows) * spacing * 0.5f;
	const float uInc = 1.0f / static_cast<float>(numCols);
	const float vInc = -1.0f / static_cast<float>(numRows);

	float w = -hpw;
	float h = -hph;
	float u = 0.0f;
	float v = 1.0f;

	Math::Vector3 norm = (horizontal) ? Math::Vector3::YAxis : -Math::Vector3::ZAxis;
	Math::Vector3 tan = Math::Vector3::XAxis;

	for (int r = 0; r <= numRows; ++r)
	{
		for (int c = 0; c <= numCols; ++c)
		{
			Math::Vector3 pos = (horizontal) ? Math::Vector3(w, 0.0f, h) : Math::Vector3(w, h, 0.0f);
			mesh.vertices.push_back({ pos, norm, tan, {u, v} });
			w += spacing;
			u += uInc;
		}
		w = -hpw;
		h += spacing;
		u = 0.0f;
		v += vInc;

	}

	CreatePlaneIndices(mesh.indices, numRows, numCols);

	return mesh;
}

MeshPC MeshBuilder::CreateCylinderPC(int slices, int rings)
{
	MeshPC mesh;
	int index = rand() % 100;

	const float hh = static_cast<float>(rings) * 0.5f;
	const float fSlices = static_cast<float>(slices);
	for (int r = 0; r <= rings; ++r)
	{
		float ring = static_cast<float>(r);
		for (int s = 0; s <= slices; ++s)
		{
			float slice = static_cast<float>(s);
			float rotation = (slice / fSlices) * Math::Constants::TwoPi;

			mesh.vertices.push_back({ {
					sin(rotation),
					ring - hh,
					-cos(rotation)},
				GetNextColor(index) });
		}
	}

	mesh.vertices.push_back({ { 0.0f, hh, 0.0f }, GetNextColor(index) });
	mesh.vertices.push_back({ { 0.0f, -hh, 0.0f }, GetNextColor(index) });

	CreatePlaneIndices(mesh.indices, rings, slices);
	CreateCapIndices(mesh.indices, slices, mesh.vertices.size() - 2, mesh.vertices.size() - 1);


	return mesh;
}

MeshPC MeshBuilder::CreateSpherePC(int slices, int rings, float radius)
{
	MeshPC mesh;
	int index = rand() % 100;

	float vertRotation = (Math::Constants::Pi / static_cast<float>(rings));
	float horRotation = (Math::Constants::TwoPi / static_cast<float>(slices));

	for (int r = 0; r <= rings; ++r)
	{
		float ring = static_cast<float>(r);
		float phi = ring * vertRotation;
		for (int s = 0; s <= slices; ++s)
		{
			float slice = static_cast<float>(s);
			float rotation = slice * horRotation;
			mesh.vertices.push_back({ {
					radius * sin(phi) * sin(rotation),
					radius * cos(phi),
					radius * sin(phi) * cos(rotation)},
				GetNextColor(index) });
		}

	}

	CreatePlaneIndices(mesh.indices, rings, slices);

	return mesh;
}

MeshPX MeshBuilder::CreateSpherePX(int slices, int rings, float radius)
{
	MeshPX mesh;
	float vertRotation = (Math::Constants::Pi / static_cast<float>(rings));
	float horRotation = (Math::Constants::TwoPi / static_cast<float>(slices));

	float uStep = 1.0f / static_cast<float>(slices);
	float vStep = 1.0f / static_cast<float>(rings);

	for (int r = 0; r <= rings; ++r)
	{
		float ring = static_cast<float>(r);
		float phi = ring * vertRotation;
		for (int s = 0; s <= slices; ++s)
		{
			float slice = static_cast<float>(s);
			float rotation = slice * horRotation;

			float u = uStep * slice;
			float v = vStep * ring;
			mesh.vertices.push_back({ {
					radius * sin(phi) * sin(rotation),
					radius * cos(phi),
					radius * sin(phi) * cos(rotation)},
					{u, v} });
		}

	}

	CreatePlaneIndices(mesh.indices, rings, slices);

	return mesh;
}

Mesh MeshBuilder::CreateSphere(int slices, int rings, float radius)
{
	Mesh mesh;

	float vertRotation = (Math::Constants::Pi / static_cast<float>(rings));
	float horzRotation = (Math::Constants::TwoPi / static_cast<float>(slices));

	float uStep = 1.0f / static_cast<float>(slices);
	float vStep = 1.0f / static_cast<float>(rings);

	for (int r = 0; r <= rings; ++r)
	{
		float ring = static_cast<float>(r);
		float phi = ring * vertRotation;
		for (int s = 0; s <= slices; ++s)
		{
			float slice = static_cast<float>(s);
			float rotation = slice * horzRotation;

			float u = uStep * slice;
			float v = vStep * ring;


			Math::Vector3 pos = {
					radius * sin(rotation) * sin(phi),
					radius * cos(phi),
					radius * cos(rotation) * sin(phi) };
			Math::Vector3 norm = Math::Normalize(pos);
			Math::Vector3 tan = abs(Math::Dot(norm, Math::Vector3::YAxis)) < 0.999f ?
				Math::Normalize({ -pos.z, 0.0f, pos.x }) : Math::Vector3::XAxis;

			mesh.vertices.push_back({ pos, norm, tan, {u, v} });
		}

	}

	CreatePlaneIndices(mesh.indices, rings, slices);

	return mesh;
}

MeshPX MeshBuilder::CreateSkyBoxSpherePX(int slices, int rings, float radius)
{
	MeshPX mesh;
	float vertRotation = (Math::Constants::Pi / static_cast<float>(rings));
	float horRotation = (Math::Constants::TwoPi / static_cast<float>(slices));

	float uStep = 1.0f / static_cast<float>(slices);
	float vStep = 1.0f / static_cast<float>(rings);

	for (int r = 0; r <= rings; ++r)
	{
		float ring = static_cast<float>(r);
		float phi = ring * vertRotation;
		for (int s = 0; s <= slices; ++s)
		{
			float slice = static_cast<float>(s);
			float rotation = slice * horRotation;

			float u = uStep * slice;
			float v = vStep * ring;
			mesh.vertices.push_back({ {
					radius * sin(phi) * cos(rotation),
					radius * cos(phi),
					radius * sin(phi) * sin(rotation)},
					{u, v} });
		}

	}

	CreatePlaneIndices(mesh.indices, rings, slices);

	return mesh;
}

MeshPX MeshBuilder::CreateOBJPX(const std::filesystem::path& filePath, float scale)
{
	MeshPX mesh;
	FILE* file = nullptr;
	fopen_s(&file, filePath.u8string().c_str(), "r");
	ASSERT(file != nullptr, "MeshBuilder: Can't open file %s", filePath.u8string().c_str());

	//read in file;
	std::vector<Math::Vector3>positions;
	std::vector<Math::Vector2> uvCoords;
	std::vector<uint32_t> positionIndices;
	std::vector<uint32_t> uvIndices;

	while (true)
	{
		char buffer[128];
		int result = fscanf_s(file, "%s", buffer, (uint32_t)std::size(buffer));
		if (result == EOF)
		{
			break;
		}
		if (strcmp(buffer, "v") == 0)
		{
			float x, y, z = 0.0f;
			fscanf_s(file, "%f %f %f\n", &x, &y, &z);
			positions.push_back({ x, y, z });
		}
		else if (strcmp(buffer, "vt") == 0)
		{
			float u, v = 0.0f;
			fscanf_s(file, "%f %f\n", &u, &v);
			uvCoords.push_back({ u, 1.0f - v });
		}
		else if (strcmp(buffer, "f") == 0)
		{
			uint32_t p[4];
			uint32_t uv[4];
			int count = fscanf_s(file, "%d/%d/%*d %d/%d/%*d %d/%d/%*d %d/%d/%*d\n", &p[0], &uv[0], &p[1], &uv[1], &p[2], &uv[2], &p[3], &uv[3]);
			if (count % 3 == 0)
			{
				for (uint32_t i = 0; i < 3; ++i)
				{
					positionIndices.push_back(p[i] - 1);
					uvIndices.push_back(uv[i] - 1);
				}
			}
			else
			{
				//triangle 1
				positionIndices.push_back(p[0] - 1);
				positionIndices.push_back(p[1] - 1);
				positionIndices.push_back(p[2] - 1);
				//triangle 2
				positionIndices.push_back(p[0] - 1);
				positionIndices.push_back(p[2] - 1);
				positionIndices.push_back(p[3] - 1);

				//triangle 1
				uvIndices.push_back(uv[0] - 1);
				uvIndices.push_back(uv[1] - 1);
				uvIndices.push_back(uv[2] - 1);
				//triangle 2
				uvIndices.push_back(uv[0] - 1);
				uvIndices.push_back(uv[2] - 1);
				uvIndices.push_back(uv[3] - 1);

			}
		}
	}
	fclose(file);
	mesh.vertices.resize(positions.size());
	for (uint32_t i = 0; i < positions.size(); ++i)
	{
		mesh.vertices[i].position = positions[i] * scale;
	}
	if (uvCoords.size() > 0)
	{
		for (uint32_t i = 0; i < uvIndices.size(); ++i)
		{
			mesh.vertices[positionIndices[i]].uvCoord = uvCoords[uvIndices[i]];

		}
	}
	mesh.indices = std::move(positionIndices);

	return mesh;
}