#include <IExeEngine/Inc/IExeEngine.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <cstdio>

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Math;

using BoneIndexMap = std::map<std::string, uint32_t>;

struct Arguments
{
	std::filesystem::path inputFileName;
	std::filesystem::path outputFileName;
	float scale = 1.0f;
};

std::optional <Arguments> ParsArgs(int argc, char* argv[])
{
	if (argc < 3)
	{
		return std::nullopt;
	}

	Arguments args;
	args.inputFileName = argv[argc - 2];
	args.outputFileName = argv[argc - 1];
	for (int i = 0; i + 2 < argc;++i)
	{
		if (strcmp(argv[i], "-scale") == 0)
		{
			args.scale = atof(argv[i + 1]);
		}
	}
	return args;
}

Vector3 ToVector3(const aiVector3D& vec)
{
	return {
		static_cast<float>(vec.x),
		static_cast<float>(vec.y),
		static_cast<float>(vec.z)
	};
}

Vector2 ToTexCoord(const aiVector3D& vec)
{
	return {
		static_cast<float>(vec.x),
		static_cast<float>(vec.y)
	};
}

Color ToColor(const aiColor3D& color)
{
	return {
		static_cast<float>(color.r),
		static_cast<float>(color.g),
		static_cast<float>(color.b),
		static_cast<float>(1.0f)
	};
}

Matrix4 ToMatrix4(const aiMatrix4x4& m)
{
	return {
		static_cast<float>(m.a1), static_cast<float>(m.b1), static_cast<float>(m.c1), static_cast<float>(m.d1),
		static_cast<float>(m.a2), static_cast<float>(m.b2), static_cast<float>(m.c2), static_cast<float>(m.d2),
		static_cast<float>(m.a3), static_cast<float>(m.b3), static_cast<float>(m.c3), static_cast<float>(m.d3),
		static_cast<float>(m.a4), static_cast<float>(m.b4), static_cast<float>(m.c4), static_cast<float>(m.d4)

	};
}

void ExportEmbeddedTexture (const aiTexture* texture, const Arguments& args,
	const std::filesystem::path& fileName)
{
	printf("Exporting embedded texture to %s...\n", fileName.u8string().c_str());
	std::string fullFileName = args.outputFileName.u8string();
	fullFileName = fullFileName.substr(0, fullFileName.rfind('/'));
	fullFileName += fileName.filename().u8string();
	FILE* file = nullptr;
	auto err = fopen_s(&file, fullFileName.c_str(), "wb");

	if(err != 0 || file == nullptr)
	{
		printf("Failed to create texture file %s\n", fullFileName.c_str());
		return;
	}
	size_t written = fwrite(texture->pcData, 1, texture->mWidth, file);
	ASSERT(written == texture->mWidth, "Failed to write texture data");
	fclose(file);

}
std::string FindTexture (const aiScene* scene, const aiMaterial* aiMaterial,
	aiTextureType textureType, const Arguments& args, const std::string& suffix,
	uint32_t materialIndex)
{
	const uint32_t textureCount = aiMaterial->GetTextureCount(textureType);
	if (textureCount == 0)
	{
		return "";
	}

	std::filesystem::path textureName;
	aiString texturePath;
	if (aiMaterial->GetTexture(textureType, 0, &texturePath) == aiReturn_SUCCESS)
	{
		if (texturePath.C_Str()[0] == '*')
		{
			std::string fileName = args.inputFileName.u8string();
			fileName.erase(fileName.length() - 4);
			fileName += suffix;
			fileName += texturePath.C_Str()[1];

			ASSERT(scene->HasTextures(), "No embedded texture");

			int textureIndex = atoi(texturePath.C_Str() + 1);
			ASSERT(textureIndex < scene->mNumTextures, "Invalid embedded texture index");

			const aiTexture* embeddedTexture = scene->mTextures[textureIndex];
			ASSERT(embeddedTexture->mHeight == 0, "ERROR: Only support compressed texture");


			ASSERT(embeddedTexture->mHeight == 0, "Only support compressed texture");

			if(embeddedTexture->CheckFormat("jpg"))
			{
				fileName += ".jpg";
			}
			else if (embeddedTexture->CheckFormat("png"))
			{
				fileName += ".png";
			}
			else
			{
				ASSERT(false, "Unsupported embedded texture format");
			}

			printf("Saving embedded texture %s...\n", fileName.c_str());
			ExportEmbeddedTexture(embeddedTexture, args, fileName);
			textureName = fileName;

		}
		else if (auto embeddedTexture = scene->GetEmbeddedTexture(texturePath.C_Str()); embeddedTexture)
		{
			std::filesystem::path embeddedFilePath = texturePath.C_Str();
			std::string fileName = args.inputFileName.u8string();
			fileName.erase(fileName.length() - 4);
			fileName += suffix;
			fileName += "_" + std::to_string(materialIndex);
			fileName += embeddedFilePath.extension().u8string();

			printf("Saving embedded texture %s...\n", fileName.c_str());
			ExportEmbeddedTexture(embeddedTexture, args, fileName);
			textureName = fileName;

		}
		else
		{
			std::filesystem::path filePath = texturePath.C_Str();
			std::string fileName = filePath.u8string();

			printf("Adding Texture %s...\n", fileName.c_str());
			textureName = fileName;
		}
	}
	
	return textureName.filename().u8string();

}

Bone* BuildSkeleton(const aiNode* seneNode, Bone* parent, Skeleton& skeleton, BoneIndexMap& boneIndexMap)
{
	Bone* bone = nullptr;
	std::string boneName = seneNode->mName.C_Str();
	auto iter = boneIndexMap.find(boneName);
	if (iter != boneIndexMap.end())
	{
		bone = skeleton.bones[iter->second].get();
	}
	else
	{
		bone = skeleton.bones.emplace_back(std::make_unique<Bone>()).get();
		bone->index = static_cast<int>(skeleton.bones.size() - 1);
		bone->offsetTransform = Matrix4::Identity;

		if (!boneName.empty())
		{
			bone->name = boneName + std::to_string(bone->index);
		}
		else
		{
			bone->name = std::move(boneName);
		}
		boneIndexMap.emplace(bone->name, bone->index);
	}
	if (skeleton.root == nullptr && parent == nullptr)
	{
		skeleton.root = bone;
	}
	bone->parent = parent;
	bone->parentIndex = parent ? parent->index : -1;
	bone->toParentTransform = ToMatrix4(seneNode->mTransformation);

	bone->children.reserve(seneNode->mNumChildren);
	for (uint32_t i = 0; i < seneNode->mNumChildren; i++)
	{
		Bone* child = BuildSkeleton(seneNode->mChildren[i], bone, skeleton, boneIndexMap);
		bone->children.push_back(child);
		bone->childrenIndices.push_back(child->index);
	}
	return bone;
}

uint32_t GetBoneIndex(const aiBone* nodeBone, const BoneIndexMap& boneIndexMap)
{
	std::string boneName = nodeBone->mName.C_Str();
	ASSERT(!boneName.empty(), "Bone name is empty");

	auto iter = boneIndexMap.find(boneName);
	ASSERT(iter != boneIndexMap.end(), "Bone not found");
	return iter->second;
}

void SetBoneOffsetTransform(const aiBone* nodeBone, Skeleton& skeleton, const BoneIndexMap& boneIndexMap)
{
	uint32_t boneIndex = GetBoneIndex(nodeBone, boneIndexMap);
	Bone* bone = skeleton.bones[boneIndex].get();
	bone->offsetTransform = ToMatrix4(nodeBone->mOffsetMatrix);
}

int main(int argc, char* argv[])
{
	const auto argOpt = ParsArgs(argc, argv);
	if (!argOpt.has_value())
	{
		printf("Not enough arguments, import fbc failed\n");
		return -1;
	}
	printf("begin Import\n");
	const Arguments& args = argOpt.value();

	Assimp::Importer importer;
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	const uint32_t flags = aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded;
	const aiScene* scene = importer.ReadFile(args.inputFileName.u8string().c_str(), flags);

	if (scene == nullptr)
	{
		printf("ERROR: %s\n", args.inputFileName.u8string().c_str());
		return -1;
	}

	printf("Importing %s...\n", args.inputFileName.u8string().c_str());

	Model model;

	BoneIndexMap boneIndexMap;


	//Matrix4 rotationMatrix = Matrix4::RotationAxis(Vector3::XAxis, 90.0f * Math::Constants::DegToRad) * Matrix4::Translation(Vector3::YAxis * 0.4f);
	if (scene->HasMeshes())
	{
		printf("ReadingSkeleton...\n");
		model.skeleton = std::make_unique<Skeleton>();
		BuildSkeleton(scene->mRootNode, nullptr, *model.skeleton, boneIndexMap);

		printf("Reading MeshData...\n");
		for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
		{
			const auto& aiMesh = scene->mMeshes[meshIndex];
			if (aiMesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
			{
				continue;
			}

			const uint32_t numVertices = aiMesh->mNumVertices;
			const uint32_t numFaces = aiMesh->mNumFaces;
			const uint32_t numIndices = numFaces * 3;

			Model::MeshData& meshData = model.meshData.emplace_back();


			printf("Reading Material Index...\n");
			meshData.materialIndex = aiMesh->mMaterialIndex;

			printf("Reading Verices...\n");
			Mesh& mesh = meshData.mesh;
			mesh.vertices.reserve(numVertices);

			const aiVector3D* positions = aiMesh->mVertices;
			const aiVector3D* normals = aiMesh->mNormals;
			const aiVector3D* tangents = (aiMesh->HasTangentsAndBitangents()) ? aiMesh->mTangents : nullptr;
			const aiVector3D* texCoords = (aiMesh->HasTextureCoords(0)) ? aiMesh->mTextureCoords[0] : nullptr;
			for (uint32_t v = 0; v < numVertices; ++v)
			{
				Vertex& vertex = mesh.vertices.emplace_back();
				vertex.position = ToVector3(positions[v]) * args.scale;
				vertex.normal = ToVector3(normals[v]);
				vertex.tangent = tangents ? ToVector3(tangents[v]) : Vector3::Zero;
				vertex.uvCoord = texCoords ? ToTexCoord(texCoords[v]) : Vector2::Zero;
			}
			//for (uint32_t v = 0; v < numVertices; ++v)
			//{
			//	Vertex& vertex = mesh.vertices[v];
			//	vertex.position = TransformCoord(vertex.position, rotationMatrix);
			//	vertex.normal = TransformNormal(vertex.normal, rotationMatrix);
			//	vertex.tangent = TransformNormal(vertex.tangent, rotationMatrix);
			//}

			printf("Reading Indices...\n");
			mesh.indices.reserve(numIndices);
			const auto& aiFaces = aiMesh->mFaces;
			for (uint32_t f = 0; f < numFaces; ++f)
			{
				const auto& aiFace = aiFaces[f];
				for (uint32_t i = 0; i < 3; ++i)
				{
					mesh.indices.push_back(aiFace.mIndices[i]);
				}
			}
		}
	}

	if (scene->HasMaterials())
	{
		printf("Reading MaterialData...\n");
		const uint32_t numMaterials = scene->mNumMaterials;
		model.materialData.reserve(numMaterials);
		for (uint32_t materialIndex = 0; materialIndex < numMaterials; ++materialIndex)
		{
			const auto& aiMaterial = scene->mMaterials[materialIndex];
			aiColor3D emissive, ambient, diffuse, specular;
			ai_real specilarPower = 10.0f;

			aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
			aiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
			aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
			aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
			aiMaterial->Get(AI_MATKEY_SHININESS, specilarPower);

			Model::MaterialData& materialData = model.materialData.emplace_back();
			materialData.material.emissive = ToColor(emissive);
			materialData.material.ambient = ToColor(ambient);
			materialData.material.diffuse = ToColor(diffuse);
			materialData.material.specular = ToColor(specular);
			materialData.material.shininess = static_cast<float>(specilarPower);

			materialData.diffuseMapName = FindTexture(scene, aiMaterial,aiTextureType_DIFFUSE,
				args, "_diff", materialIndex);
			materialData.specMapName = FindTexture(scene, aiMaterial, aiTextureType_DIFFUSE,
				args, "_spec", materialIndex);
			materialData.normalMapName = FindTexture(scene, aiMaterial, aiTextureType_NORMALS,
				args, "_norm", materialIndex);
			materialData.bumpMapName = FindTexture(scene, aiMaterial, aiTextureType_HEIGHT, 
				args, "_bump", materialIndex);


		}
	}


	printf("Saving Model...\n");
	ModelIO::SaveModel(args.outputFileName, model);

	printf("Saving Material...\n");
	ModelIO::SaveMaterial(args.outputFileName, model);

	printf("Finished!\n");
	return 0;
}