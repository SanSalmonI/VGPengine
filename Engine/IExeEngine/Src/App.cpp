#include "Precompiled.h"
#include "App.h"

using namespace IExeEngine;
using namespace IExeEngine::Core;

void App::Run()
{
	LOG("App Started");

	mRunning = true;
	while (mRunning)
	{
		LOG("Running");
	}

	LOG("App Quit");
}

void App::Quit()
{

}