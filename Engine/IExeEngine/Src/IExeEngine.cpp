#include "Precompiled.h"
#include "IExeEngine.h"

IExeEngine::App& IExeEngine::MainApp()
{
	static App sApp;
	return sApp;
}