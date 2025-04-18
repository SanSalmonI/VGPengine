#pragma once

namespace IExeEngine
{
	class App final
	{
	public:
		void Run();

		void Quit();

	private:
		bool mRunning = false;
	};
}