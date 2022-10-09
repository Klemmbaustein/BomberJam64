#include "Console.h"
#include <WorldParameters.h>
#include <Rendering/Utility/CSM.h>
#include <Sound/Sound.h>
#include <Engine.h>
#include <sstream>
#include <Rendering/Utility/ShaderManager.h>
#include <map>
#include <Script.h>
#include <regex>
#include <Math/Math.h>
#include <Rendering/Camera/CameraShake.h>
#include <Log.h>
#include <Importers/Build/Pack.h>
#include <OS.h>
std::map<std::string, std::string> Variables;
namespace Application
{
	void Quit();
}
struct ForceQuitException : std::exception
{
	const char* what() const noexcept
	{
		return "Quit forcefully";
	}
};
std::vector<std::string> CommandsToTry = { "info", "define {varname} = {val}", "echo {text}", "runscript {script file name}" };
std::string LastConsoleString = "";
std::string ConsoleHeader = "[CONSOLE]: ";
bool IsInIf = false;
bool CanExecute = true;
Vector3 ErrorColor = Vector3(1, 0.5, 0.5);
bool Console::ExecuteConsoleCommand(std::string Command, bool Verbose)
{
	if (CanExecute)
	{
		size_t startbracket = Command.find_first_of('('), endbracket = Command.find_last_of(')');

		std::string OriginalCommand = Command;
		for (std::pair<std::string, std::string> v : Variables)
		{
			Command = std::regex_replace(Command, std::regex(v.first), v.second);
		}
		if (startbracket != std::string::npos && endbracket != std::string::npos)
		{
			Command.replace(startbracket, endbracket, std::to_string(Maths::SolveExpr(Command.substr(startbracket, endbracket - startbracket))));
		}
		const char* ptr = Command.c_str();
		std::string CurrentName, CurrentVal;
		bool ReadName = false;
		bool LastFileCommentedout = false, RanCommand = false;
		while (ptr[0] != '\0')
		{
			if (ptr[0] != '/')
			{
				if (ptr[0] != ' ')
				{
					if (!ReadName)
					{
						if (ptr[0] != '=')
						{
							CurrentName = CurrentName + ptr[0];
						}
						else
						{
							ReadName = true;
						}
					}
					else
					{
						CurrentVal = CurrentVal + ptr[0];
					}
				}
			}
			else if (LastFileCommentedout)
			{
				break;
			}
			else LastFileCommentedout = true;
			ptr++;
		}
		if (!CurrentName.empty())
		{
			if (CurrentName == "PCFQuality")
			{
				RanCommand = true;
				try
				{
					Graphics::PCFQuality = std::stoi(CurrentVal);
					if (Verbose)
						Log::CreateNewLogMessage(ConsoleHeader + "Value set: " + CurrentName + "=" + CurrentVal);
				}
				catch (std::exception)
				{
					Log::CreateNewLogMessage(ConsoleHeader + "Invalid argument: " + CurrentVal + " (Expected integer)", ErrorColor);
				}
			}
			if (CurrentName == "ShadowResolution")
			{
				RanCommand = true;
				try
				{
					Graphics::ShadowResolution = std::stoi(CurrentVal);
					if (Verbose)
						Log::CreateNewLogMessage(ConsoleHeader + "Value set: " + CurrentName + "=" + CurrentVal);
					CSM::ReInit();
				}
				catch (std::exception)
				{
					Log::CreateNewLogMessage(ConsoleHeader + "Invalid argument: " + CurrentVal + " (Expected integer)", ErrorColor);
				}
			}
			if (CurrentName == "TimeMultiplier")
			{
				RanCommand = true;
				try
				{
					if (CurrentVal == "Yes" || CurrentVal == "yes")
					{
						Performance::TimeMultiplier = INFINITY; // <- Oh no;
					}
					else Performance::TimeMultiplier = std::stof(CurrentVal);
					if (Verbose)
						Log::CreateNewLogMessage(ConsoleHeader + "Value set: " + CurrentName + "=" + CurrentVal);
				}
				catch (std::exception)
				{
					Log::CreateNewLogMessage(ConsoleHeader + "Invalid argument: " + CurrentVal + " (Expected float)", ErrorColor);
				}
			}
			if (CurrentName == "Gamma")
			{
				RanCommand = true;
				try
				{
					if (CurrentVal == "Yes" || CurrentVal == "yes")
					{
						Graphics::Gamma = INFINITY; // <- Oh no;
					}
					else Graphics::Gamma = std::stof(CurrentVal);
					if (Verbose)
						Log::CreateNewLogMessage(ConsoleHeader + "Value set: " + CurrentName + "=" + CurrentVal);
				}
				catch (std::exception)
				{
					Log::CreateNewLogMessage(ConsoleHeader + "Invalid argument: " + CurrentVal + " (Expected float)", ErrorColor);
				}
			}
			if (CurrentName == "Bloom")
			{
				RanCommand = true;
				try
				{
					Graphics::Bloom = CurrentVal == "true" ? true : false;
					if (Verbose)
						Log::CreateNewLogMessage(ConsoleHeader + "Value set: " + CurrentName + "=" + CurrentVal);
				}
				catch (std::exception)
				{
					Log::CreateNewLogMessage(ConsoleHeader + "Invalid argument: " + CurrentVal + " (Expected bool)", ErrorColor);
				}
			}
			if (CurrentName == "ChromaticAbberation")
			{
				RanCommand = true;
				try
				{
					if (CurrentVal == "Yes" || CurrentVal == "yes")
					{
						Graphics::ChrAbbSize = INFINITY; // <- Oh no;
					}
					else Graphics::ChrAbbSize = std::stof(CurrentVal) / 100;
					if (Verbose)
						Log::CreateNewLogMessage(ConsoleHeader + "Value set: " + CurrentName + "=" + CurrentVal);
				}
				catch (std::exception)
				{
					Log::CreateNewLogMessage(ConsoleHeader + "Invalid argument: " + CurrentVal + " (Expected float)", ErrorColor);
				}
			}
			if (CurrentName == "Vignette")
			{
				RanCommand = true;
				try
				{
					if (CurrentVal == "Yes" || CurrentVal == "yes")
					{
						Graphics::Vignette = INFINITY; // <- Oh no;
					}
					else Graphics::Vignette = std::stof(CurrentVal) / 10;
					if (Verbose)
						Log::CreateNewLogMessage(ConsoleHeader + "Value set: " + CurrentName + "=" + CurrentVal);
				}
				catch (std::exception)
				{
					Log::CreateNewLogMessage(ConsoleHeader + "Invalid argument: " + CurrentVal + " (Expected float)", ErrorColor);
				}
			}
			if (CurrentName == "FXAA")
			{
				RanCommand = true;
				try
				{
					Graphics::FXAA = CurrentVal == "true" ? true : false;
					if (Verbose)
						Log::CreateNewLogMessage(ConsoleHeader + "Value set: " + CurrentName + "=" + CurrentVal);
				}
				catch (std::exception)
				{
					Log::CreateNewLogMessage(ConsoleHeader + "Invalid argument: " + CurrentVal + " (Expected bool)", ErrorColor);
				}
			}
			if (CurrentName == "camshake")
			{
				RanCommand = true;
				
				try
				{
					if (CurrentVal == "Yes" || CurrentVal == "yes")
					{
						CameraShake::PlayDefaultCameraShake(INFINITY); // <- Very much Oh no;
					}
					else CameraShake::PlayDefaultCameraShake(std::stof(CurrentVal));
					if (Verbose)
						Log::CreateNewLogMessage(ConsoleHeader + "Played Camera Shake: " + CurrentVal);
				}
				catch (std::exception)
				{
					Log::CreateNewLogMessage(ConsoleHeader + "Invalid argument: " + CurrentVal + " (Expected float)", ErrorColor);
				}
			}
			if (CurrentName.substr(0, 4) == "echo")
			{
				RanCommand = true;
				if (CurrentName.size() > 4)
				{
					if (CurrentName[4] == '*' && CurrentName.size() < 6)
					{
						Log::CreateNewLogMessage(ConsoleHeader + "Last command: \"" + LastConsoleString + "\"");
					}
					else
					{
						Log::CreateNewLogMessage(ConsoleHeader + Command.substr(5));
					}
				}
				else
				{
					Log::CreateNewLogMessage(ConsoleHeader + "Missing argument: " + Command + " <- (Expected string)", ErrorColor);
					Log::CreateNewLogMessage(ConsoleHeader + "Example usage: \"echo hello world\"", ErrorColor);
				}
			}
			if (CurrentName == "*")
			{
				RanCommand = true;
				Log::CreateNewLogMessage(ConsoleHeader + "Last command: \"" + LastConsoleString + "\"");
				return true;
			}
			if (CurrentName == "sndlist" || CurrentName == "snd" || CurrentName == "sndinfo")
			{
				Log::CreateNewLogMessage(ConsoleHeader + "--------------------------------------------------------------------------");
				RanCommand = true;
				std::vector<std::string> sounds = Sound::GetSounds();
				Log::CreateNewLogMessage(ConsoleHeader + "Sound List: " + std::to_string(sounds.size()) + "/255");
				for (std::string s : sounds)
				{
					Log::CreateNewLogMessage(ConsoleHeader + s);
				}
				Log::CreateNewLogMessage(ConsoleHeader + "--------------------------------------------------------------------------");
			}
			if (CurrentName.substr(0, 9) == "playsound")
			{
				RanCommand = true;
				auto s = Sound::LoadSound(CurrentName.substr(9));
				Sound::PlaySound2D(s);
				delete s;
				if (Verbose)
					Log::CreateNewLogMessage(ConsoleHeader + "Played Sound \"" + CurrentName.substr(9) + "\"");
			}
			if (CurrentName == "help")
			{
				RanCommand = true;
				Log::CreateNewLogMessage(ConsoleHeader + "Console help:", Vector3(1, 1, 0));
				Log::CreateNewLogMessage(ConsoleHeader + "Commands to try:", Vector3(1, 1, 0));
				for (std::string c : CommandsToTry)
				{
					Log::CreateNewLogMessage(ConsoleHeader + "    " + c);
				}
				Log::CreateNewLogMessage(ConsoleHeader + "more info:", Vector3(1, 1, 0));
				Log::CreateNewLogMessage(ConsoleHeader + "\"*\" shows last registered command", Vector3(1, 1, 0));
				Log::CreateNewLogMessage(ConsoleHeader + "the config file executes commands on startup", Vector3(1, 1, 0));
			}
			if (CurrentName == "info" || CurrentName == "?")
			{
				RanCommand = true;
				Log::CreateNewLogMessage(ConsoleHeader + "--------------------------------------------------------------------------");
				Log::CreateNewLogMessage(ConsoleHeader + "Info:");
				Log::CreateNewLogMessage(ConsoleHeader + "Version: " + std::string(VERSION_STRING) + (IS_IN_EDITOR ? "-Editor (" : " (") + std::string(ProjectName) + ")");
				Log::CreateNewLogMessage(ConsoleHeader + "OS: " + OS::GetOSString());
				Log::CreateNewLogMessage(ConsoleHeader + "Window Resolution: x=" + std::to_string((int)Graphics::WindowResolution.X) + " y=" + std::to_string((int)Graphics::WindowResolution.Y));
				Log::CreateNewLogMessage(ConsoleHeader + "Sounds: " + std::to_string(Sound::GetSounds().size()) + "/255");
				Log::CreateNewLogMessage(ConsoleHeader + "Shadow Resolution: " + std::to_string(Graphics::ShadowResolution) + "x" + std::to_string(Graphics::ShadowResolution) + "px");
				Log::CreateNewLogMessage(ConsoleHeader + "OpenGL Version: " + std::string((const char*)glGetString(GL_VERSION)) + " GLSL: " + std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));
				Log::CreateNewLogMessage(ConsoleHeader + "OpenAL Version: " + Sound::GetVersionString());
				Log::CreateNewLogMessage(ConsoleHeader + "Shaders: " + std::to_string(GetNumShaders()));
				Log::CreateNewLogMessage(ConsoleHeader + "--------------------------------------------------------------------------");
			}

			if (OriginalCommand == "version")
			{
				RanCommand = true;
				Log::CreateNewLogMessage(ConsoleHeader + "Engine Version: " + std::string(VERSION_STRING) + (IS_IN_EDITOR ? "-Editor (" : " (") + std::string(ProjectName) + ")");
				Log::CreateNewLogMessage(ConsoleHeader + "OpenGL Version: " + std::string((const char*)glGetString(GL_VERSION)) + " GLSL: " + std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));
				Log::CreateNewLogMessage(ConsoleHeader + "OpenAL Version: " + Sound::GetVersionString());
			}
			if (OriginalCommand == "stopshake")
			{
				CameraShake::StopAllCameraShake();
				RanCommand = true;
			}

			if (OriginalCommand.substr(0, 8) == "listpack")
			{
				std::string Pack = OriginalCommand.substr(9);
				RanCommand = true;
				auto Result = Pack::GetPackContents(Pack);
				for (Pack::PackFile i : Result)
				{
					Log::CreateNewLogMessage(ConsoleHeader + i.FileName);
					std::cout << i.FileName << std::endl << i.Content << std::endl;
				}
				if (Result.size() == 0)
				{
					Log::CreateNewLogMessage(ConsoleHeader + "Pack File is emtpy");
				}
			}

			if (OriginalCommand.substr(0, 6) == "define")
			{
				RanCommand = true;
				std::string varname, varvalue, equals;
				std::stringstream stream = std::stringstream(OriginalCommand);
				stream >> varname;
				stream >> varname >> equals;
				while (!stream.eof())
				{
					std::string outval;
					stream >> outval;
					varvalue = varvalue + outval;
					if (!stream.eof())
						varvalue = varvalue + " ";
				}
				if (equals == "=")
				{
					if (Variables.contains(varname))
					{
						Variables.erase(Variables.find(varname));
					}
					Variables.insert(std::make_pair(varname, varvalue));
					if (Verbose)
						Log::CreateNewLogMessage(ConsoleHeader + "Defined " + varname + " as " + varvalue);
				}
				else
				{
					if (Verbose)
						Log::CreateNewLogMessage(ConsoleHeader + "Missing equals sign: Example usage: \"define Test = Test message\"", ErrorColor);
				}
			}
			if (OriginalCommand.substr(0, 6) == "undef ")
			{
				RanCommand = true;
				if (Variables.contains(OriginalCommand.substr(6)))
				{
					Variables.erase(Variables.find(OriginalCommand.substr(6)));
					Log::CreateNewLogMessage(ConsoleHeader + "Undefined " + OriginalCommand.substr(6));
				}
				else Log::CreateNewLogMessage(ConsoleHeader + "Could not undefine " + OriginalCommand.substr(6), ErrorColor);
			}
			if (OriginalCommand.substr(0, 7) == "#define")
			{
				Log::CreateNewLogMessage(ConsoleHeader + "This is not C++");
			}
			if (OriginalCommand == "run *")
			{
				Log::CreateNewLogMessage(ConsoleHeader + "ran " + LastConsoleString);
				return ExecuteConsoleCommand(LastConsoleString, Verbose);
			}
			if (OriginalCommand == "forcequit" || OriginalCommand == "fquit")
			{
				throw ForceQuitException();
			}
			if (OriginalCommand == "quit")
			{
				Application::Quit();
				RanCommand = true;
			}
			if (OriginalCommand == "undefall")
			{
				RanCommand = true;
				Variables.clear();
				Log::CreateNewLogMessage(ConsoleHeader + "Undefined everything");
			}
			if (OriginalCommand == "listdef")
			{
				RanCommand = true;
				Log::CreateNewLogMessage(ConsoleHeader + "--------------------------------------------------------------------------");
				Log::CreateNewLogMessage(ConsoleHeader + "All definitions:");
				for (std::pair<std::string, std::string> v : Variables)
				{
					Log::CreateNewLogMessage(ConsoleHeader + v.first + " = " + v.second);
				}
				Log::CreateNewLogMessage(ConsoleHeader + "--------------------------------------------------------------------------");
			}
			if (OriginalCommand == "public static void main(String Args[])")
			{
				RanCommand = true;
				Log::CreateNewLogMessage(ConsoleHeader + "Boilerplate", ErrorColor);
			}
			if (Command.substr(0, 3) == "if ")
			{
				RanCommand = true;
				IsInIf = true;
				std::stringstream stream = std::stringstream(Command);
				std::string statement;
				stream >> statement;
				stream >> statement;
				std::string eqCommand = std::regex_replace(Command.substr(3), std::regex(" "), "");
				size_t equals = eqCommand.find("=");
				if (equals != std::string::npos)
				{
					std::string preeq = eqCommand.substr(0, equals);
					std::string posteq = eqCommand.substr(equals + 1);
					if (preeq == posteq)
					{
						CanExecute = true;
					}
					else
					{
						CanExecute = false;
					}
				}
				else if (statement == "true" || statement == "1")
				{
					CanExecute = true;
				}
				else if (statement == "false" || statement == "0")
				{
					CanExecute = false;
				}
				else
				{
					Log::CreateNewLogMessage(ConsoleHeader + statement + " is not a valid if statement", ErrorColor);
					IsInIf = false;
				}
				if (Verbose && IsInIf)
				{
					Log::CreateNewLogMessage(ConsoleHeader + (CanExecute ? "true" : "false"));
				}
			}
			if (OriginalCommand == "loadcfg")
			{
				RanCommand = true;
				Config::LoadConfigs();
			}
			if (OriginalCommand.substr(0, 9) == "runscript")
			{
				RanCommand = true;
				if (OriginalCommand.size() == 9)
				{
					Log::CreateNewLogMessage(ConsoleHeader + "Expected an identifier", ErrorColor);
					Log::CreateNewLogMessage(ConsoleHeader + "Example: runscript myscript", ErrorColor);
				}
				else
				{
					Script::LoadScript((OriginalCommand.substr(10)).c_str());
				}
			}
			if (IsInIf)
			{
				if (Command == "endif")
				{
					IsInIf = false;
					CanExecute = true;
					if (Verbose)
						Log::CreateNewLogMessage(ConsoleHeader + "ended if");
					return true;
				}
				if (Command == "else")
				{
					CanExecute = !CanExecute;
					if (Verbose)
					{
						Log::CreateNewLogMessage(ConsoleHeader + "else");
					}
				}
			}
			if (Verbose && !RanCommand)
				Log::CreateNewLogMessage(ConsoleHeader + "Unrecognised command: " + Command.substr(0, Command.find_first_of(' ')), ErrorColor);
			LastConsoleString = OriginalCommand;
		}
		return RanCommand;
	}
	else
	{
		if (Command == "endif")
		{
			IsInIf = false;
			CanExecute = true;
			if (Verbose)
			Log::CreateNewLogMessage(ConsoleHeader + "ended if");
			return true;
		}
		if (Command == "else")
		{
			CanExecute = !CanExecute;
			if (Verbose)
			{
				Log::CreateNewLogMessage(ConsoleHeader + "else");
			}
			return true;
		}
		if (Verbose)
		{
			Log::CreateNewLogMessage(ConsoleHeader + "Cannot execute, is in if statement");
		}
	}
	return false;
}
