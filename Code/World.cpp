#pragma once
#include "World.h"
#include "FileUtility.h"
#include <filesystem>
#include <Objects/Objects.h>
#include <sstream>
#include <Log.h>
#include <Sound/Sound.h>
namespace fs = std::filesystem;



namespace World
{
	bool ShouldLoadNewScene = false;
	std::string NewLoadedScene;
	Camera* DefaultCamera = new Camera(2, 1600, 900, false);

	std::string CurrentScene = "Content/NewScene";
	void LoadNewScene(std::string FilePath)
	{
		std::string OriginalString = FilePath;
		FilePath = Assets::GetAsset(FilePath + ".jscn");
		if (fs::exists(FilePath))
		{
			Graphics::MainCamera = DefaultCamera;
			if (!IsInEditor)
			{
				for (UICanvas* b : Graphics::UIToRender)
				{
					delete b;
				}
				Graphics::UIToRender.clear();
			}	
			Sound::StopAllSounds();
			for (int i = 0; i < Objects::AllObjects.size(); i++)
			{
				Objects::AllObjects.at(i)->IsSelected = false;
			}
			TextInput::PollForText = false;
			Debugging::EngineStatus = "Loading Scene";
			for (int i = 0; i < Objects::AllObjects.size(); i++)
			{
				if (Objects::AllObjects[i] != nullptr)
				{
					Objects::DestroyObject(Objects::AllObjects[i]);
				}
				else
				{
				}
			}
			Objects::AllObjects.clear();
			Graphics::ModelsToRender.clear();
			Collision::CollisionBoxes.clear();
			std::ifstream Input(FilePath, std::ios::in | std::ios::binary);
			std::vector<WorldObject> WorldObjects;
			Editor::IsInSubscene = false;
			CurrentScene = GetFilePathWithoutExtension(FilePath);

			if (fs::is_empty(FilePath))
			{
				Log::CreateNewLogMessage("Loaded Scene (Scene File is empty)");
				return;
			}
			int ObjectLength = 0;
			Input.read((char*)&ObjectLength, sizeof(int));

			for (int i = 0; i < ObjectLength; i++)
			{
				if (!Input.eof())
				{
					Transform Transform1;
					Input.read((char*)&Transform1, sizeof(Transform));
					uint32_t ID = 0;
					Input.read((char*)&ID, sizeof(uint32_t));
					std::string Name;
					std::string Path;
					std::string desc;
					int len = 0;
					Input.read((char*)&len, sizeof(int));
					char* temp = new char[len + 1];
					Input.read(temp, len);
					temp[len] = '\0';
					Name = temp;
					delete[] temp;

					Input.read((char*)&len, sizeof(int));
					temp = new char[len + 1];
					Input.read(temp, len);
					temp[len] = '\0';
					Path = temp;
					delete[] temp;
					Input.read((char*)&len, sizeof(int));
					temp = new char[len + 1];
					Input.read(temp, len);
					temp[len] = '\0';
					desc = temp;
					delete[] temp;
					WorldObject* NewObject = Objects::SpawnObjectFromID(ID, Transform1);
					NewObject->Deserialize(Path);
					NewObject->SetName(Name);
					NewObject->LoadProperties(desc);
					NewObject->OnPropertySet();
					NewObject->CurrentScene = CurrentScene;
				}
			}
			Log::CreateNewLogMessage(std::string("Loaded Scene (").append(std::to_string(ObjectLength)).append(std::string(" Object(s) Loaded)")));
			Input.close();
		}
		else
		{
			Log::CreateNewLogMessage("Scene Loading Error: Scene \"" + OriginalString + "\" does not exist");
		}
	}

	void SaveSceneAs(std::string FilePath, bool Subscene)
	{
		Debugging::EngineStatus = "Saving Scene";
		std::ofstream Output(FilePath + (Subscene ? ".subscn" : ".jscn"), std::ios::out | std::ios::binary);
		std::vector<WorldObject*> SavedObjects;
		for (WorldObject* o : Objects::AllObjects)
		{
			if (o->CurrentScene == CurrentScene)
			{
				SavedObjects.push_back(o);
			}
		}
		int ObjectLength = SavedObjects.size();
		Output.write((char*)&ObjectLength, sizeof(int));

		for (int i = 0; i < ObjectLength; i++)
		{
				std::string name = SavedObjects.at(i)->GetName();
				std::string path = SavedObjects.at(i)->Serialize();
				uint32_t ID = SavedObjects[i]->GetObjectDescription().ID; //C++ 20 or not?
				Transform T = SavedObjects[i]->GetTransform();
				Output.write((char*)&T, sizeof(Transform));
				Output.write((char*)&ID, sizeof(uint32_t));
				int len = name.size();
				Output.write((char*)&len, sizeof(int));
				Output.write(name.c_str(), len);

				len = path.size();
				Output.write((char*)&len, sizeof(int));
				Output.write(path.c_str(), len);

				std::string Desc = SavedObjects[i]->GetPropertiesAsString();
				len = Desc.size();
				Output.write((char*)&len, sizeof(int));
				Output.write(Desc.c_str(), len);
		}

		Output.close();
	}
	void LoadSubScene(std::string FilePath)
	{
		FilePath = Assets::GetAsset(FilePath + ".subscn");
		for (int i = 0; i < Objects::AllObjects.size(); i++)
		{
			Objects::AllObjects.at(i)->IsSelected = false;
		}
		Editor::IsInSubscene = true;
		TextInput::PollForText = false;
		Debugging::EngineStatus = "Loading Subscene";
		if (fs::exists(FilePath))
		{
			std::ifstream Input(FilePath, std::ios::in | std::ios::binary);
			std::vector<WorldObject> WorldObjects;
			CurrentScene = GetFilePathWithoutExtension(FilePath);

			if (fs::is_empty(FilePath))
			{
				Log::CreateNewLogMessage("Loaded Subscene (Scene File is empty)");
				return;
			}
			int ObjectLength = 0;
			Input.read((char*)&ObjectLength, sizeof(int));

			for (int i = 0; i < ObjectLength; i++)
			{
				if (!Input.eof())
				{
					Transform Transform1;
					Input.read((char*)&Transform1, sizeof(Transform));
					uint32_t ID = 0;
					Input.read((char*)&ID, sizeof(uint32_t));
					std::string Name;
					std::string Path;
					std::string desc;
					int len = 0;
					Input.read((char*)&len, sizeof(int));
					char* temp = new char[len + 1];
					Input.read(temp, len);
					temp[len] = '\0';
					Name = temp;
					delete[] temp;

					Input.read((char*)&len, sizeof(int));
					temp = new char[len + 1];
					Input.read(temp, len);
					temp[len] = '\0';
					Path = temp;
					delete[] temp;
					Input.read((char*)&len, sizeof(int));
					temp = new char[len + 1];
					Input.read(temp, len);
					temp[len] = '\0';
					desc = temp;
					delete[] temp;
					WorldObject* NewObject = Objects::SpawnObjectFromID(ID, Transform1);
					NewObject->Deserialize(Path);
					NewObject->SetName(Name);
					NewObject->LoadProperties(desc);
					NewObject->OnPropertySet();
					NewObject->CurrentScene = CurrentScene;
				}
			}
			Log::CreateNewLogMessage(std::string("Loaded Subscene (").append(std::to_string(ObjectLength)).append(std::string(" Object(s) Loaded)")));
			Input.close();
		}
	}
	void Tick()
	{
		if (ShouldLoadNewScene)
		{
			World::LoadNewScene(NewLoadedScene);
			ShouldLoadNewScene = false;
		}
	}
	void ScheduleLoadNewScene(std::string FilePath)
	{
		NewLoadedScene = FilePath;
		ShouldLoadNewScene = true;
	}
}