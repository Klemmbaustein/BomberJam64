#include "Save.h"
#include <fstream>
#include <filesystem>
#include <Log.h>
#include <sstream>

std::string Types[] =
{
	"null",
	"vector3",
	"float",
	"int",
	"string"
};

SaveGame::SaveGame(std::string SaveName)
{
	SaveName.append(".save");
	OpenedSave = SaveName;

	//if a save does not exist yet, it will be created on deconstruction.
	if (std::filesystem::exists(SaveName))
	{
		std::ifstream InFile = std::ifstream(SaveName, std::ios::in);

		char CurrentBuff[100];

		//iterate through all lines which (hopefully) contain save values
		while (!InFile.eof())
		{
			TypeEnum CurrentType = T_NULL;
			std::string CurrentName = "unkown";
			std::string Value = "null";

			std::string CurrentLine;
			InFile.getline(CurrentBuff, 100);
			CurrentLine = CurrentBuff;
			std::stringstream CurrentLineStream = std::stringstream(CurrentLine);

			//if the current line is empty, we ignore it
			if (!CurrentLine.empty())
			{

				std::string Type;
				CurrentLineStream >> Type;
				for (unsigned int i = 0; i < 5; i++)
				{
					if (Types[i] == Type)
					{
						CurrentType = (TypeEnum)((int)i - 1);
					}
				}
				if (CurrentType == T_NULL)
				{
					Log::CreateNewLogMessage("Error reading save file: " + Type + " is not a valid type (" + CurrentLine + ")", Vector3(1, 0, 0));
				}
				std::string Equals;

				CurrentLineStream >> CurrentName;
				CurrentLineStream >> Equals;

				if (Equals != "=")
				{
					Log::CreateNewLogMessage("Error reading save file: expected = sign (" + CurrentLine + ")", Vector3(1, 0, 0));
				}
				//the rest of the stream is the value of the save item
				while(!CurrentLineStream.eof())
				{
					std::string ValueToAppend;
					CurrentLineStream >> ValueToAppend;
					Value.append(ValueToAppend);
				}

				Properties.insert(std::pair(CurrentName, SaveProperty(CurrentName, Value, CurrentType)));
			}
		}
		InFile.close();
	}
}

SaveGame::SaveProperty SaveGame::GetPropterty(std::string Name)
{
	auto FoundIndex = Properties.find(Name);
	if (FoundIndex != Properties.end())
	{
		return Properties.find(Name)->second;
	}
	return SaveProperty();
}

void SaveGame::SetPropterty(SaveProperty S)
{
	if (Properties.find(S.Name) != Properties.end())
	{
		Properties[S.Name] = S;
		return;
	}
	else
	{
		Properties.insert(std::pair(S.Name, S));
	}
}

SaveGame::~SaveGame()
{
	std::ofstream OutFile = std::ofstream(OpenedSave, std::ios::out);

	//loop through all the properties and write them to the "OpenedSave" variable
	for (auto p : Properties)
	{
		OutFile << Types[p.second.Type + 1];
		OutFile << " ";
		OutFile << p.second.Name;
		OutFile << " = ";
		OutFile << p.second.Value;
		OutFile << std::endl;
	}
	OutFile.close();
}

SaveGame::SaveProperty::SaveProperty(std::string Name, std::string Value, TypeEnum Type)
{
	this->Name = Name;
	this->Value = Value;
	this->Type = Type;
}
