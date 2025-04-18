#include "Precompiled.h"
#include "App.h"

using namespace IExeEngine;
using namespace IExeEngine::Core;

void App::Run(const AppConfig& config)
{
	LOG("App Started");

	// Initialize Everything
	Window myWindow;
	myWindow.Initialize(
		GetModuleHandle(nullptr),
		config.appName,
		config.winWidth,
		config.winHeight
		);

	// Process Updates
	mRunning = true;
	while (mRunning)
	{
		myWindow.ProcessMessage();
		
		if (!myWindow.IsActive())
		{
			Quit();
			continue;
		}
	}

	// Terminate Everything
	LOG("App Quit");

	myWindow.Terminate();
}

void App::Quit()
{
	mRunning = false;
}