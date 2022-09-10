#include "Build.h"
#include <filesystem>
#include <WorldParameters.h>
#include <Importers/Build/Pack.h>
#include <Log.h>
#include <Engine.h>
#include <fstream>
#include <sstream>

constexpr const char SolutionName[] = "BomberJam64";

std::string Build::TryBuildProject(std::string TargetFolder)
{
	try
	{
		if (std::filesystem::exists(TargetFolder))
		{
			Debugging::EngineStatus = "Build: Clearing folder";
			Log::CreateNewLogMessage("Build: Clearing folder");

			for (const auto& entry : std::filesystem::directory_iterator(TargetFolder))
				std::filesystem::remove_all(entry.path());

			Debugging::EngineStatus = "Build: Copying .dll files";
			Log::CreateNewLogMessage("Build: Copying .dll files");

			std::filesystem::copy("SDL2.dll", TargetFolder + "SDL2.dll");
			std::filesystem::copy("OpenAL32.dll", TargetFolder + "OpenAL32.dll");

			Debugging::EngineStatus = "Build: Creating folders";
			Log::CreateNewLogMessage("Build: Creating folders");

			std::filesystem::create_directories(TargetFolder + "/Assets/Content");

			Debugging::EngineStatus = "Build: Packaging shaders";
			Log::CreateNewLogMessage("Build: Packaging shaders");
			Pack::SaveFolderToPack("Shaders/", TargetFolder + "/Assets/shaders.pack");

			Debugging::EngineStatus = "Build: Copying assets";
			Log::CreateNewLogMessage("Build: Copying assets");
			const auto copyOptions = std::filesystem::copy_options::recursive;
			std::filesystem::copy("Content", TargetFolder + "/Assets/Content", copyOptions);
			std::filesystem::copy("Fonts", TargetFolder + "Assets/");
			std::filesystem::copy("Config.jss", TargetFolder + "Assets/Config.jss");

			Debugging::EngineStatus = "Building C++ solution";
			Log::CreateNewLogMessage("Build: Generating code");

			std::ifstream EngineFile = std::ifstream("Code/Engine.h", std::ios::in);

			std::stringstream EngineHeaderString;
			std::stringstream OriginalHeaderString;

			unsigned int i = 0;
			while (!EngineFile.eof())
			{
				i++;
				char addedbuf[100];
				EngineFile.getline(addedbuf, 100);
				if (i == 3)
				{
					EngineHeaderString << "#define IS_IN_EDITOR false";
				}
				else if (i == 5)
				{
					EngineHeaderString << "#define ENGINE_DEBUG false";
				}
				else
				{
					EngineHeaderString << addedbuf;
				}
				if (!EngineFile.eof())
				{
					EngineHeaderString << std::endl;
				}
			}
			EngineFile.close();
			EngineFile.open("Code/Engine.h", std::ios::in);
			OriginalHeaderString << EngineFile.rdbuf();
			EngineFile.close();
			std::ofstream OutH = std::ofstream("Code/Engine.h", std::ios::out);
			OutH << EngineHeaderString.str();
			OutH.close();
			Log::CreateNewLogMessage("Build: Compiling Code (this can take a while)");
			system(("G:/Data/VisualStudio/Common7/IDE/devenv.exe " + std::string(SolutionName) + ".sln /Build Release-EngineBuild").c_str());

			std::filesystem::copy("x64/Release-EngineBuild/" + std::string(SolutionName) + ".exe", TargetFolder + ProjectName + std::string(".exe"));
			Log::CreateNewLogMessage("Build: Cleaning up");
			OutH.open("Code/Engine.h", std::ios::out);
			OutH << OriginalHeaderString.str();
			OutH.close();
			Log::CreateNewLogMessage("Build: Complete", Vector3(0, 1, 0));
			return "Sucess";
		}
		Log::CreateNewLogMessage("Build: Cannot find folder", Vector3(1, 0, 0));
		std::filesystem::create_directories(TargetFolder);
		return Build::TryBuildProject(TargetFolder);
	}
	catch (std::exception& e)
	{
		Log::CreateNewLogMessage(std::string("Build: ") + e.what(), Vector3(1, 0, 0));
		return "Error";
	}
}
/*
* Engine.h
#pragma once
//WARNING: IS_IN_EDITOR must always be on line 3 for the build to work
#define IS_IN_EDITOR true
#define VERSION_STRING "0.1"
#define ENGINE_DEBUG true
constexpr char ProjectName[] = "TestGame";
*/