#pragma once


namespace IExeEngine::Graphics
{
	struct Model;
	class Animation;

	class AnimationIO
	{
	public:
		static void Write(FILE* file, const Animation& animation);
		static void Read(FILE* file, Animation& animation);
	};


	namespace ModelIO
	{
		void SaveModel(std::filesystem::path filePath, const Model& model);
		void LoadModel(std::filesystem::path filePath, Model& model);

		void SaveMaterial(std::filesystem::path directory, const Model& model);
		void LoadMaterial(std::filesystem::path directory, Model& model);

		void SaveSkeleton(std::filesystem::path filePath, const Model& model);
		void LoadSkeleton(std::filesystem::path filePath, Model& model);

		void SaveAnimation(std::filesystem::path filePath, const Model& model);
		void LoadAnimation(std::filesystem::path filePath, Model& model);
	}
}