#pragma once

#include "MeshBuffer.h"
#include "Transform.h"
#include "Material.h"
#include "TextureManager.h"

namespace IExeEngine::Graphics
{
	class RenderObject
	{
	public:
		void Terminate();

		Transform transform;  // location/orientation
		MeshBuffer meshBuffer;// shape
		Material material;    // light data
		TextureId diffuseMapId; //diffuse texture for an object
		TextureId specMapId;  //specular map for an object
		TextureId normalMapId; //normal texture for an object
		TextureId bumpMapId;   //bump texture for an object
	};
}
