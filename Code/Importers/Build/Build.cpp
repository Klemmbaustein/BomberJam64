#include "Build.h"
#include <filesystem>
#include <WorldParameters.h>
#include <Importers/Build/Pack.h>
#include <Log.h>
#include <Engine.h>
#include <fstream>
#include <sstream>

#define VS_INSTALL_PATH "G:/Data/VisualStudio/Common7/IDE/devenv.exe "

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

#if _WIN32

			Log::CreateNewLogMessage("Build: Compiling Code (this can take a while)");
			system((VS_INSTALL_PATH + std::string(SolutionName) + ".sln /Build FinalBuild").c_str());
			std::filesystem::copy("x64/FinalBuild/Engine.exe", TargetFolder + ProjectName + std::string(".exe"));
			Log::CreateNewLogMessage("Build: Cleaning up");
#else
			Log::CreateNewLogMessage("Build: Compiling is currently not supported on Linux.", Vector3(1, 0, 0));
			Log::CreateNewLogMessage("Pleasse recompile the program manually with IS_IN_EDITOR and ENGINE_DEBUG as false.", Vector3(1, 0, 0));
#endif
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