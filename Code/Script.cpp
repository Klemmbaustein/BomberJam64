#include "Script.h"
#include <fstream>
#include <iostream>
#include <Console.h>
#include "Script.h"
#include <FileUtility.h>
#include <filesystem>
#include <Log.h>
#include <World/Stats.h>
#include <World/Assets.h>

extern const bool IsInEditor;
extern const bool EngineDebug;

void Config::LoadConfigs()
{
	Debugging::EngineStatus = "Reading Config";
	Log::CreateNewLogMessage("Running Script \"Config\"");

	std::string Path = (IsInEditor || EngineDebug) ? "Config.jss" : "Assets/Config.jss";


	std::ifstream File = std::ifstream(Path, std::ios::in);
	while (!File.eof())
	{
		char* CurrentLine = new char[100];

		File.getline(CurrentLine, 100);
		Console::ExecuteConsoleCommand(CurrentLine);
	}
}

void Script::LoadScript(const char* Name)
{

	std::string Path = Assets::GetAsset(Name + std::string(".jss")); //JavaScriptScript, not to be confused with JavaScript
	Log::CreateNewLogMessage("Running Script \"" + GetFilePathWithoutExtension(Name) + "\"");

	if (std::filesystem::exists(Path))
	{
		Debugging::EngineStatus = "Reading Script" + std::string(Name) + std::string(".jss");
		std::ifstream File = std::ifstream(Path, std::ios::in);
		while (!File.eof())
		{
			char* CurrentLine = new char[100];

			File.getline(CurrentLine, 100);
			Console::ExecuteConsoleCommand(CurrentLine);
		}
	}
	else
	{
		Log::CreateNewLogMessage("Error: Could not read Script \"" + std::string(Name) + std::string(".jss\""), Vector3(1, 0, 0));
	}
}