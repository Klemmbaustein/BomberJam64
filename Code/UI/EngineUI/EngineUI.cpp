#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX

#include "EngineUI.h"
#include "Utility/stb_image.h"
#include "FileUtility.h"
#include <filesystem>
#include <algorithm>
#include "Math/Math.h"
#include <fstream>
#include "Math/Collision/Collision.h"
#include "Importers/ModelConverter.h"
#include "Importers/ImageImporter.h"
#include "Objects/MeshObject.h"
#include "World.h"
#include <Input.h>
#include <Console.h>
#include <Objects/Objects.h>
#include <Log.h>
#include <OS.h>
#include <thread>
#include <Importers/Build/Build.h>
namespace fs = std::filesystem;
namespace EngineUIVariables
{
	std::vector<Tab> Tabs;
}

//Parameters for the Arrows. Putting them right here in the cpp because "What could possibly go wrong?"

Collision::Box ArrowBoxX
(
	-1.f, 0.f,
	-0.1f, 0.1f,
	-0.1f, 0.1f

);

Collision::Box ArrowBoxY
(
	-0.1f, 0.1f,
	0.f, 1.f,
	-0.1f, 0.1f

);

Collision::Box ArrowBoxZ
(
	-0.1f, 0.1f,
	-0.1f, 0.1f,
	-1.f, 0.f
	
);
bool Copied = false;
bool UserDraggingButton = false;

namespace ContentBrowser
{

	std::string CurrentFilePath = "Content";

	std::vector<Asset> ContentAssets;
	void UpdateContentAssets() //Search for assets in path
	{
		ContentAssets.clear();
		std::filesystem::path Path(CurrentFilePath);
		if (std::filesystem::exists(Path))
		{
			for (const auto& entry : std::filesystem::directory_iterator(Path))
			{
				if (std::filesystem::is_directory(entry.path()))
				{
					Asset NewAsset = Asset();
					NewAsset.FilePath = std::string(entry.path().string());
					NewAsset.IsDirectory = true;
					ContentAssets.push_back(NewAsset);
				}
			}
			for (const auto& entry : std::filesystem::directory_iterator(Path))
			{
				if (!std::filesystem::is_directory(entry.path()))
				{
					Asset NewAsset = Asset();
					NewAsset.FilePath = std::string(entry.path().string());
					NewAsset.IsDirectory = false;
					ContentAssets.push_back(NewAsset);
				}
			}
		}
		else
		{
			Log::CreateNewLogMessage(std::string("Error: Content File Path does not exist! - Resetting File Path..."), Vector3(1, 0.1, 0.1));
			CurrentFilePath = "Content";
		}
	}
}

bool ChangedScene = false;
bool Dragging = false;
Vector3 PreviosLocation;
Vector3 Axis;
int SelectedTab = 0;

bool AddItemDrowdownHovered = false;

void CreateNewFile(std::string Name) //Creates a new file. Will append a number if the file already exists
{
	if (!fs::is_regular_file(Name) && !fs::is_character_file(Name) && !fs::is_other(Name))
	{
		std::fstream Out(Name, std::ios::out);
		Out.close();
		return;
	}
	int AppendedNumber = 0;
	while(true)
	{
		std::string NewName = GetFilePathWithoutExtension(Name);
		NewName.append(std::to_string(AppendedNumber));
		std::string Ext = Name.substr(Name.find_last_of(".") + 1);
		NewName.append(".").append(Ext);
		if (!fs::is_regular_file(NewName))
		{
			std::fstream Out(NewName, std::ios::out);
			Out.close();
			break;
		}
		AppendedNumber++;
	}
}

GLuint GetTextureFromFilePath(std::string Path, std::vector<GLuint> Textures) //Returns the TextureID from the given Path
{
	
	if (fs::is_directory(Path))
	{
		return Textures[1];
	}
	std::string Ext = Path.substr(Path.find_last_of(".") + 1);
	if (Ext == "jsm")				//3d Model
	{
		return Textures[0];
	}
	else if (Ext == "cpp")			//C++ File (Don't know what this is doing here)
	{
		return Textures[2];
	}
	else if (Ext == "txt")			//Text File (Also don't know why this is here)
	{
		return Textures[4];
	}
	else if (Ext == "png")			//Image
	{
		return Textures[6];
	}
	else if (Ext == "jscn")			//Scene
	{
		return Textures[10];
	}
	else if (Ext == "subscn")		//Subscene (Basically a Scene but it can be loaded as a subscene)
	{
		return Textures[14];
	}
	else if (Ext == "jsmat")		//Material
	{
		return Textures[12];
	}
	else if (Ext == "jsmtmp")		//Material Template
	{
		return Textures[13];
	}
	else if (Ext == "jss")
	{
		return Textures[4];
	}
	else if (Ext == "wav")
	{
		return Textures[15];
	}
	return Textures.at(3);			//Not recognised
}

std::vector<WorldObject*> SelectedObjects;
size_t LastConsoleLength;

//Clicking And Dragging

EngineUI::EngineUI() : UICanvas()
{
	//Create the default Tab
	EngineUIVariables::Tabs.push_back(EngineUIVariables::Tab(0, "Viewport", "", false));

	//Scan for assets
	ContentBrowser::UpdateContentAssets();
	UpdateContentBrowser();
	//Initialize all the UI Elements

	UIElements.push_back(new UIBorder(Vector2(1.f, 0.93f), Vector2(-2.f, -0.2f), Vector3(0.14f, 0.14f, 0.15f)));
	UIElements.push_back(new UIBorder(Vector2(1.f, -1.f), Vector2(-2.f, 0.4f), Vector3(0.14f, 0.14f, 0.15f)));
	UIElements.push_back(new UIBorder(Vector2(1.f, 0.93f), Vector2(-0.3f, -2.f), Vector3(0.14f, 0.14f, 0.15f)));
	UIElements.push_back(new UIBorder(Vector2(-1.f, 0.93f), Vector2(0.3f, -2.f), Vector3(0.14f, 0.14f, 0.15f)));
	UIElements.push_back(new UIBorder(Vector2(-1.f, 1.f), Vector2(2.f, -0.07f), Vector3(0.1f, 0.1f, 0.11f)));
	UIElements.push_back(new UIBorder(Vector2(-0.7f, -0.63f), Vector2(1.375f, -0.35f), Vector3(0.1f, 0.1f, 0.11f)));
	UIElements.push_back(new UIBorder(Vector2(-0.7f, 0.73f), Vector2(1.4f, -0.05f), Vector3(0.075f, 0.075f, 0.1f)));

	UIElements.push_back(new UIButton(Vector2(-0.85f, -0.77f), Vector2(0.085f, 0.05f), Vector3(1), this, 56));
	UIElements.push_back(new UIButton(Vector2(-0.95f, -0.77f), Vector2(0.085f, 0.05f), Vector3(1), this, 57));
	ConsoleTextField = new TextField(Vector2(-0.7f, -0.99f), Vector2(1.375f, 0.05f), Vector3(0.2f, 0.2f, 0.21f), &EngineUITextRenderer, this, 58);
	UIElements.push_back(ConsoleTextField);
	ContextMenu = new EngineContextMenu(&EngineUITextRenderer);
	CreateNewButton(Vector2(-0.98f, 0.8f), Vector2(0.13f, 0.075f), Vector3(0.4f, 1.f, 0.4f), 50);
	CreateNewButton(Vector2(-0.83f, 0.8f), Vector2(0.13f, 0.075f), Vector3(0.4f, 1.f, 0.4f), 51);
	UIButton* ContentBrowserBackButton = new UIButton(Vector2(-0.94f, 0.785f), Vector2(-0.05f, -0.05f), Vector3(1), this, 52, false, Textures.at(5), true);
	UIElements.push_back(ContentBrowserBackButton);
	UIButton* SaveButton = new UIButton(Vector2(-0.55f, 0.9f), Vector2(-0.09f / 2, -0.16f / 2), Vector3(1), this, 53, false, Textures.at(7), true);
	UIElements.push_back(SaveButton);

	UIElements.push_back(new UIButton(Vector2(-0.45f, 0.9f), Vector2(-0.09f / 2, -0.16f / 2), Vector3(1), this, 59, false, Textures.at(8), true));

	TabWidgets.push_back(nullptr);
	TabWidgets.push_back(new MaterialTemplate(Vector2(), Vector2(1), Vector3(1), &EngineUITextRenderer, Textures.at(7)));
	TabWidgets.push_back(new MaterialUI(Vector2(), Vector2(1), Vector3(1), &EngineUITextRenderer, Textures.at(7), Textures.at(8)));
	TabWidgets.push_back(new MeshTab(&EngineUITextRenderer));
	TabWidgets.push_back(new AddNewItemTab(&EngineUITextRenderer, this));
	//Create cursors
	CrossCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
	GrabCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	DefaultCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
}

void(*QuitF)();
void EngineUI::OnLeave(void(*ReturnF)())
{
	QuitF = ReturnF;
	if (ChangedScene)
	{
		ShowPopUpWindow("Save Changes to scene?",
			{
				PopUpButton("Yes", true, &EngineUI::SaveChangesAndLeave),
				PopUpButton("No", true, &EngineUI::Leave),
				PopUpButton("Cancel", false, nullptr)
			});
	}
	else Leave();
}

void EngineUI::ShowPopUpWindow(std::string Message, std::vector<PopUpButton> Buttons)
{
	PopUpMesage = Message;
	PopUpUI.clear();
	PopUpWindow = true;
	PopUpButtons = Buttons;
	PopUpUI.push_back(new UIBorder(Vector2(-0.25), Vector2(0.5, 0.3), Vector3(0.1)));
	for (uint32_t i = 0; i < Buttons.size(); i++)
	{
		PopUpUI.push_back(new UIButton(Vector2(-0.19 + (i / 7.f), -0.2), Vector2(0.1, 0.06f), Vector3(1), this, -1 - i));
	}
}

void EngineUI::Leave()
{
	(*QuitF)();
}

void EngineUI::SaveChangesAndLeave()
{
	World::SaveSceneAs(World::CurrentScene, Editor::IsInSubscene);
	(*QuitF)();
}



void EngineUI::OnButtonClicked(int Index)
{
	Debugging::EngineStatus = "EngineUIRendering: OnButtonClicked";
	if (Index == 50) //Add new file dropdown
	{
		AddNewItemDropDown = true;
		DoSafeUpdate = true;
	}
	else if (Index == 51) //Add item dialog
	{
		if (!UserDraggingButton)
		{
			std::string FilePath = OS::ShowOpenFileDialog();
			std::string Ext = FilePath.substr(FilePath.find_last_of(".") + 1);
			if (Ext == "fbx" || Ext == "obj")
				ModelImporter::Import(FilePath, ContentBrowser::CurrentFilePath);
			else if (Ext == "png" || Ext == "wav")
				ImageImporter::Import(FilePath, ContentBrowser::CurrentFilePath);
			DoSafeUpdate = true;
		}
	}
	else if (Index == 52) //File path back arrow... thing?
	{
		if (!UserDraggingButton)
		{
			if (!(ContentBrowser::CurrentFilePath.find("/") == std::string::npos))
			{
				size_t lastindex = ContentBrowser::CurrentFilePath.find_last_of("/");
				std::string rawname = ContentBrowser::CurrentFilePath.substr(0, lastindex);
				ContentBrowser::CurrentFilePath = rawname;
				DoSafeUpdate = true;
			}
		}
		else
		{
			size_t lastindex = ContentBrowser::CurrentFilePath.find_last_of("/");
			std::string rawname = ContentBrowser::CurrentFilePath.substr(0, lastindex);
			if (std::rename(ContentBrowser::ContentAssets.at(DraggedButton).FilePath.c_str(),
				(rawname).append("/").append(GetFileNameFromPath(ContentBrowser::ContentAssets.at(DraggedButton).FilePath)).c_str()) < 0)
				Log::CreateNewLogMessage(std::string("Error moving File: ").append(strerror(errno)), Vector3(1, 0.3f, 0.3f));
			DoSafeUpdate = true;
		}
	}
	else if (Index == 53) //Save scene (or subscene)
	{
		World::SaveSceneAs(World::CurrentScene, Editor::IsInSubscene);
		ChangedScene = false;
		DoSafeUpdate = true;
	}
	else if (Index == 54) //Recompile shaders (unused)
	{
		Graphics::RecompileShaders();
		DoSafeUpdate = true;
	}
	else if (Index == 56) //Content browser mode: C++ Classes
	{
		Log::CreateNewLogMessage("Content Browser Mode: Classes");
		ContentBrowserMode = true;
		DoSafeUpdate = true;
	}
	else if (Index == 57) //Content browser mode: Normal
	{
		Log::CreateNewLogMessage("Content Browser Mode: Normal");
		ContentBrowserMode = false;
		DoSafeUpdate = true;
	}
	else if (Index == 58) //Console command text field
	{
		if(Input::IsKeyDown(SDLK_RETURN))
		Console::ExecuteConsoleCommand(ConsoleTextField->Text, true);
		ConsoleTextField->Text.clear();
	}
	else if (Index == 59)
	{
		if (BuildThread)
		{
			BuildThread->detach();
			delete BuildThread;
		}
		BuildThread = new std::thread(Build::TryBuildProject, "Build/");
	}
	else if (Index == 70) //Create new folder in dropdown menu
	{
		std::string Path = ContentBrowser::CurrentFilePath;
		fs::create_directory(ContentBrowser::CurrentFilePath.append("/Folder/"));
		AddNewItemDropDown = false;
		ContentBrowser::CurrentFilePath = Path;
		DoSafeUpdate = true;
	}
	else if (Index == 71) //Create new scene in dropdown menu
	{
		std::string Path = ContentBrowser::CurrentFilePath;
		CreateNewFile(ContentBrowser::CurrentFilePath.append("/Scene.jscn"));
		AddNewItemDropDown = false;
		ContentBrowser::CurrentFilePath = Path;
		DoSafeUpdate = true;
	}
	else if (Index == 72) //Other in dropdown menu
	{
		EngineUIVariables::Tabs.push_back(EngineUIVariables::Tab(4, "Add New Item",
			ContentBrowser::CurrentFilePath));
		DoSafeUpdate = true;
		AddNewItemDropDown = false;
	}
	else if (Index >= 200) //List of objects
	{
		for (int i = 0; i < Objects::AllObjects.size(); i++)
		{
			Objects::AllObjects.at(i)->IsSelected = false;
		}
		Objects::AllObjects.at(Index - 200)->IsSelected = true;
		ContextMenu->SetObject(Objects::AllObjects.at(Index - 200));
	}
	else if (Index < ContentBrowser::ContentAssets.size() && Index >= 0) //Content browser assets
	{
		if (!UserDraggingButton && !ContentBrowserMode)
		{
			std::string Ext = ContentBrowser::ContentAssets.at(Index).FilePath.substr(ContentBrowser::ContentAssets.at(Index).FilePath.find_last_of(".") + 1);
			if (fs::is_directory(ContentBrowser::ContentAssets.at(Index).FilePath))
			{
				ContentBrowser::CurrentFilePath = ContentBrowser::CurrentFilePath + "/" + (GetFileNameFromPath(ContentBrowser::ContentAssets.at(Index).FilePath));
				DoSafeUpdate = true;
			}
			else if (Ext == "cpp" || Ext == "h" || Ext == "txt" || Ext == "jss") //Text files..?
			{
				OS::OpenFile(fs::canonical(ContentBrowser::ContentAssets.at(Index).FilePath).string());
			}
			else if (Ext == "jscn") //Scene
			{
				SelectedObjects.clear();
				ContextMenu->SetObject(nullptr);
				ContextMenu->Generate();
				World::LoadNewScene(GetFileNameWithoutExtensionFromPath(ContentBrowser::ContentAssets.at(Index).FilePath));
				ChangedScene = false;
				DoSafeUpdate = true;
			}
			else if (Ext == "subscn") //Subscene
			{
				World::LoadSubScene(GetFileNameWithoutExtensionFromPath(ContentBrowser::ContentAssets[Index].FilePath));
				ChangedScene = false;
				DoSafeUpdate = true;
			}
			else if (Ext == "jsmtmp") //Material Template
			{
				EngineUIVariables::Tabs.push_back(EngineUIVariables::Tab(1, GetFileNameWithoutExtensionFromPath(ContentBrowser::ContentAssets.at(Index).FilePath),
					ContentBrowser::ContentAssets.at(Index).FilePath));
				DoSafeUpdate = true;
			}
			else if (Ext == "jsmat") //Material
			{
				EngineUIVariables::Tabs.push_back(EngineUIVariables::Tab(2, GetFileNameWithoutExtensionFromPath(ContentBrowser::ContentAssets.at(Index).FilePath),
					ContentBrowser::ContentAssets.at(Index).FilePath));
				DoSafeUpdate = true;
			}
			else if (Ext == "jsm")   //3d Model
			{
				EngineUIVariables::Tabs.push_back(EngineUIVariables::Tab(3, GetFileNameWithoutExtensionFromPath(ContentBrowser::ContentAssets.at(Index).FilePath),
					ContentBrowser::ContentAssets.at(Index).FilePath));
				DoSafeUpdate = true;
				ContextMenu->SetObject(nullptr);
			}
			else if (Ext == "png")   //Pictue
			{
				OS::OpenFile(fs::canonical(ContentBrowser::ContentAssets.at(Index).FilePath).string());
			}
			else if (Ext == "wav")   //Sound file
			{
				Console::ExecuteConsoleCommand("playsound " + GetFileNameWithoutExtensionFromPath(ContentBrowser::ContentAssets.at(Index).FilePath), true);
			}
		}
		else
		{
			std::string Ext = ContentBrowser::ContentAssets.at(Index).FilePath.substr(ContentBrowser::ContentAssets.at(Index).FilePath.find_last_of(".") + 1);

			if (fs::is_directory(ContentBrowser::ContentAssets.at(Index).FilePath) && Index != DraggedButton)
			{
				GetFileNameFromPath(ContentBrowser::ContentAssets.at(DraggedButton).FilePath);
				if (std::rename(ContentBrowser::ContentAssets.at(DraggedButton).FilePath.c_str(),
				(ContentBrowser::ContentAssets.at(Index).FilePath).append("/").
					append(GetFileNameFromPath(ContentBrowser::ContentAssets.at(DraggedButton).FilePath)).c_str()) < 0)
				Log::CreateNewLogMessage(std::string("Error moving File: ").append(strerror(errno)), Vector3(1, 0.3f, 0.3f));
				DoSafeUpdate = true;
			}
		}
	}
	else if (Index < 0 && Index > -4) // Pop up menu
	{
		for (int i = 0; i < PopUpUI.size(); i++)
		{
			delete PopUpUI[i];
		}
		PopUpUI.clear();
		if (PopUpButtons[(0 - Index) - 1].CallFunc)
		{
			void(EngineUI::*ButtonPtr)() = PopUpButtons[(0 - Index) - 1].FPointer;
			(*this.*ButtonPtr)();
		}
		PopUpWindow = false;
	}
	else if (Index < -200) //Tabs
	{
		if (Index % 2 == 0)
		{
			if (TabWidgets.at(EngineUIVariables::Tabs[(Index + 300) / 2].Type) == nullptr)
			{
				if (TabWidgets.at(EngineUIVariables::Tabs.at(SelectedTab).Type) != nullptr)
				{
					if (fs::exists(EngineUIVariables::Tabs[SelectedTab].Path))
					{
						TabWidgets.at(EngineUIVariables::Tabs.at(SelectedTab).Type)->Save();
					}
				}
				SelectedTab = (Index + 300) / 2;
				DoSafeUpdate = true;
			}
			else if (fs::exists(EngineUIVariables::Tabs[(Index + 300) / 2].Path))
			{
				if (TabWidgets.at(EngineUIVariables::Tabs.at(SelectedTab).Type) != nullptr)
				{
					if (fs::exists(EngineUIVariables::Tabs[SelectedTab].Path))
					{
						TabWidgets.at(EngineUIVariables::Tabs.at(SelectedTab).Type)->Save();
					}
				}
				SelectedTab = (Index + 300) / 2;
				TabWidgets.at(EngineUIVariables::Tabs.at(SelectedTab).Type)->Load(EngineUIVariables::Tabs.at(SelectedTab).Path);
				DoSafeUpdate = true;
			}
		}
		else
		{
			Index--;
			EngineUIVariables::Tabs.erase(EngineUIVariables::Tabs.begin() + ((Index + 300) / 2));
			DoSafeUpdate = true;
		}
	}
	Assets::ScanForAssets();
}

void EngineUI::OnButtonDragged(int Index)
{
	if (!UserDraggingButton)
	{
		DraggedButton = Index;
		UserDraggingButton = true;
	}
}


//Rendering and Updating

void EngineUI::Render(Shader* Shader)
{
	std::vector<bool> NewButtonsPressed;
	std::vector<bool> NewButtonsSelected;
	Vector2 PrevMousePos;
	int j = 0;
	if (PopUpWindow)
	{
		PrevMousePos = Input::MouseLocation; // Weird hack to prevent the mouse from interacting with the background
		Input::MouseLocation = 999;
	}
	SelectedObjects.clear();
	for (int i = 0; i < Objects::AllObjects.size(); i++)
	{
		if (WorldButtons.size() > i && Objects::AllObjects.at(i) != nullptr)
		{
			Shader->Bind();
			Vector3 Color;
			if (!Objects::AllObjects.at(i)->IsSelected)
			{
				Color = Vector3(0.1f, 0.1f, 0.115f);
			}
			else
			{
				Color = Vector3(0.2f, 0.2f, 0.2f);
			}
			WorldButtons.at(i)->Color = Color;
			WorldButtons.at(i)->Render(Shader);
			if (i != RenamedIndex || !RenamingWorldObject)
			{
				EngineUITextRenderer.RenderText(Objects::AllObjects[i]->GetName(),
					Vector2(0.77f, 0.679f - (i / 16.f)), 0.9f, Vector3(1), &WorldObjectsScrollObject);
			}
			else
			{
				EngineUITextRenderer.RenderText(TextInput::Text,
					Vector2(0.77f, 0.679f - (i / 16.f)), 0.9f, Vector3(1), &WorldObjectsScrollObject);
			}
			EngineUITextRenderer.RenderText(Objects::AllObjects[i]->GetObjectDescription().Name + " in "
				+ GetFileNameWithoutExtensionFromPath(Objects::AllObjects[i]->CurrentScene),
				Vector2(0.77f, 0.659f - (i / 16.f)), 0.7f, Vector3(1), &WorldObjectsScrollObject);
			if (Objects::AllObjects.at(i)->IsSelected)
			{
				SelectedObjects.push_back(Objects::AllObjects.at(i));
				if (Input::IsKeyDown(SDLK_F2))
				{
					RenamingContentAsset = false;
					RenamingWorldObject = true;
					RenamedIndex = i;
					TextInput::PollForText = true;
					TextInput::Text = Objects::AllObjects[i]->GetName();
				}
			}
		}
	}
	if (!TextInput::PollForText && RenamingWorldObject) // Rename file
	{
		Objects::AllObjects[RenamedIndex]->SetName(TextInput::Text);
		RenamingWorldObject = false;
		RenamingContentAsset = false;
	}
	if (ContentBrowserMode)
	{
		try
		{
			for (int i = 0; i < ContentButtons.size(); i++)
			{
				ContentButtons[i]->Render(Shader);
				EngineUITextRenderer.RenderText(Objects::EditorObjects[i].Name,
					Vector2((-0.85f + ((i % 2) / 7.f)) - 2.f / 16, (0.7f - ((int)(i / 2) / 4.f)) - 1.9f / 9), 1.f, Vector3(1),
					&ContentBrowserScrollObject);
			}
		}
		catch (std::exception) {}
	}
	else
	{
		try
		{
			for (int i = 0; i < ContentBrowser::ContentAssets.size(); i++)
			{
				if (i != RenamedIndex || !RenamingContentAsset)
				{
					ContentButtons[i]->Render(Shader);
					EngineUITextRenderer.RenderText(GetFileNameWithoutExtensionFromPath(ContentBrowser::ContentAssets[i].FilePath),
						Vector2((-0.85f + ((i % 2) / 7.f)) - 2.f / 16, (0.7f - ((int)(i / 2) / 4.f)) - 1.9f / 9), 1.f, Vector3(1), &ContentBrowserScrollObject);
					if (ContentButtons[i]->Selected && Input::IsKeyDown(SDLK_F2)) // Start renaming because f2 is pressed and the button is selected
					{
						RenamingWorldObject = false;
						RenamingContentAsset = true;
						RenamedIndex = i;
						TextInput::PollForText = true;
						TextInput::Text = GetFileNameWithoutExtensionFromPath(ContentBrowser::ContentAssets[i].FilePath);
					}
					else if (ContentButtons[i]->Selected && Input::IsKeyDown(SDLK_DELETE))
					{
						DoSafeUpdate = true;
						std::filesystem::remove_all(ContentBrowser::ContentAssets[i].FilePath);
					}
				}
				else
				{
					ContentButtons[i]->Render(Shader);
					EngineUITextRenderer.RenderText(TextInput::Text,
						Vector2((-0.85f + ((i % 2) / 7.f)) - 2.f / 16, (0.7f - ((int)(i / 2) / 4.f)) - 1.9f / 9), 1.f, Vector3(1), &ContentBrowserScrollObject);
				}
			}
			if (!TextInput::PollForText && RenamingContentAsset) //Rename file
			{
				if (ContentBrowser::ContentAssets[RenamedIndex].IsDirectory) //Asset is Directory
				{
					std::string NewPath = ContentBrowser::ContentAssets[RenamedIndex].FilePath;
					size_t LastSlash = NewPath.find_last_of('/');
					size_t LastBackSlash = NewPath.find_last_of('\\'); //Get Path
					if (LastSlash == std::string::npos)
					{
						if (LastBackSlash == std::string::npos)
							throw UIRenderingException("EngineUI", "Invalid Rename string: No slashes in path");
						NewPath = NewPath.substr(0, LastBackSlash);
					}
					else if (LastBackSlash == std::string::npos)
					{
						NewPath = NewPath.substr(0, LastSlash);
					}
					else
					{
						NewPath = NewPath.substr(0, std::max(LastSlash, LastBackSlash));
					}
					fs::rename(ContentBrowser::ContentAssets[RenamedIndex].FilePath, NewPath + "/" + TextInput::Text);
				}
				else //Asset is File
				{
					std::string NewPath = ContentBrowser::ContentAssets[RenamedIndex].FilePath;
					size_t LastSlash = NewPath.find_last_of('/');
					size_t LastBackSlash = NewPath.find_last_of('\\'); //Get Path
					if (LastSlash == std::string::npos)
					{
						if (LastBackSlash == std::string::npos)
							throw UIRenderingException("EngineUI", "Invalid Rename string: No slashes in path");
						NewPath = NewPath.substr(0, LastBackSlash);
					}
					else if (LastBackSlash == std::string::npos)
					{
						NewPath = NewPath.substr(0, LastSlash);
					}
					else
					{
						NewPath = NewPath.substr(0, std::max(LastSlash, LastBackSlash));
					}
					//Append new name and extension to path
					NewPath = NewPath + "/" + TextInput::Text + "." + GetExtension(ContentBrowser::ContentAssets[RenamedIndex].FilePath);
					fs::rename(ContentBrowser::ContentAssets[RenamedIndex].FilePath, NewPath);
				}
				RenamingContentAsset = false;
				DoSafeUpdate = true;
			}
		}
		catch(std::exception) {}
	}

	Debugging::EngineStatus = "Rendering (Tab bar)";
	for (int i = 0; i < EngineUIVariables::Tabs.size(); i++)
	{
		if ((EngineUIVariables::Tabs.size()) > SelectedTab)
		{
			if (i * 2 < TabButtons.size())
			{
				if (TabButtons[i * 2] != nullptr)
				{
					Debugging::EngineStatus = i * 2 > TabButtons.size() ? "true" : "false";
					TabButtons.at(i * 2)->Render(Shader);
					EngineUITextRenderer.RenderText(EngineUIVariables::Tabs.at(i).Name, TabButtons.at(i * 2)->Position + Vector2(0.01f), 1.f, Vector3(1));
					TabButtons.at((i * 2) + 1)->Render(Shader);
				}
			}
		}
	}
	Debugging::EngineStatus = "Rendering (Tabs)";

	if ((EngineUIVariables::Tabs.size()) > SelectedTab)
	{
		if (TabWidgets.at(EngineUIVariables::Tabs.at(SelectedTab).Type) != nullptr)
		{
			TabWidgets.at(EngineUIVariables::Tabs.at(SelectedTab).Type)->Render(Shader);
		}
	}
	if (LastConsoleLength != Log::Messages.size())
	{
		LogScrollObject.Percentage = std::max(((int)Log::Messages.size() - 10) / 40.f, 0.f);
		LastConsoleLength = Log::Messages.size();
	}
	for (unsigned int i = 0; i < Log::Messages.size(); i++)
	{
		EngineUITextRenderer.RenderText(std::string(Log::Messages.at(i).Ammount != 0 ?
			Log::Messages.at(i).Text + std::string(" (x") + std::to_string(Log::Messages.at(i).Ammount + 1) + std::string(")") :
			Log::Messages.at(i).Text),
			Vector2(-0.69f, -0.675f - (float)i / 40), 0.9f, Log::Messages.at(i).Color, &LogScrollObject);
	}
	Debugging::EngineStatus = "Rendering (EngineUI: Text)";

	EngineUITextRenderer.RenderText(std::string("FPS = " + std::to_string((int)Performance::FPS)), Vector2(-0.98, 0.95), 1.7f, Vector3(1));
	EngineUITextRenderer.RenderText(
		std::string(
			"OBJ: " + std::to_string(Objects::AllObjects.size())+ 
			", DRW: " + std::to_string(Performance::DrawCalls) + "-" + std::to_string(Graphics::ModelsToRender.size()) +
			", CLL: " + std::to_string(Collision::CollisionBoxes.size())),
		Vector2(-0.7, 0.95), 1.7f, Vector3(0.5));

	EngineUITextRenderer.RenderText(Engine::CurrentProjectName, Vector2(-0.83, 0.97), 1.f, Vector3(0.75));
	EngineUITextRenderer.RenderText(Engine::VersionString, Vector2(-0.83, 0.945), 1.f, Vector3(0.75));

	EngineUITextRenderer.RenderText(ContentBrowser::CurrentFilePath, Vector2(-0.93f, 0.745f), 1.f, Vector3(0.8f));

	EngineUITextRenderer.RenderText(std::string("Add new item"), Vector2(-0.9685f, 0.83f), 1.f, Vector3(0));
	EngineUITextRenderer.RenderText(std::string("Import"), Vector2(-0.81f, 0.83f), 1.f, Vector3(0));
	EngineUITextRenderer.RenderText(std::string("Save Scene"), Vector2(-0.62f, 0.79f), 1.f, Vector3(1));
	EngineUITextRenderer.RenderText(std::string("Build"), Vector2(-0.5f, 0.79f), 1.f, Vector3(1));
	EngineUITextRenderer.RenderText(std::string(GetFileNameWithoutExtensionFromPath(World::CurrentScene)), Vector2(-0.62f, 0.77f), 0.75f, Vector3(1));
	EngineUITextRenderer.RenderText(std::string("Modes"), Vector2(-0.95f, -0.7f), 1.f, Vector3(1));
	EngineUITextRenderer.RenderText(std::string("C++ Classes"), Vector2(-0.845f, -0.76f), 0.8f, Vector3(0));
	EngineUITextRenderer.RenderText(std::string("Content"), Vector2(-0.945f, -0.76f), 0.8f, Vector3(0));
	Debugging::EngineStatus = "Rendering (EngineUI)";

	if (Maths::IsPointIn2DBox(Vector2(0.7f, 0.68f), Vector2(-0.7f, -0.6f), Input::MouseLocation) && TabWidgets[EngineUIVariables::Tabs[SelectedTab].Type] == nullptr)
	{
		SDL_SetCursor(CrossCursor);
	}
	else
	{
		SDL_SetCursor(DefaultCursor);
	}
	if (Input::IsKeyDown(SDLK_DELETE))
	{
		if (ContextMenu->CurrentObject)
		{
			ContextMenu->SetObject(nullptr);
			ContextMenu->Generate();
		}
	}
	NewButtonsPressed.clear();

	ContextMenu->Render(Shader);

	if (AddNewItemDropDown)
	{
		AddItemDrowdownHovered = false;
		std::string Options[]
		{
			"Add Folder",
			"Add Scene",
			"Other"
		};
		for (int i = 0; i < 3; i++)
		{
			Shader->Bind();
			UIButton NewButton(Vector2(-0.98f, 0.725f - ((float)i / 15.f)), Vector2(0.3f, 0.075f), Vector3(0.1f), this, 70 + i, false);
			NewButton.IsButtonDown = AddItemsButtonsPressed.at(i);
			NewButton.Render(Shader);
			if(NewButton.IsHovered)
				AddItemDrowdownHovered = true;
			NewButtonsPressed.push_back(NewButton.IsButtonDown);
			EngineUITextRenderer.RenderText(Options[i], Vector2(-0.97f, 0.75f - ((float)i / 15.f)), 0.9f, Vector3(1));
		}
		AddItemsButtonsPressed = NewButtonsPressed;
	}

	if (Input::IsLMBDown && AddNewItemDropDown && !AddItemDrowdownHovered) AddNewItemDropDown = false;
	Vector3 DistanceScaleMultiplier;
	Vector3 Rotation = Graphics::MainCamera->ForwardVectorFromScreenPosition(Input::MouseLocation.X * 1.4f, ((Input::MouseLocation.Y * 1.5f) - 0.1f));
	if(SelectedObjects.size() > 0)
		DistanceScaleMultiplier = Vector3((SelectedObjects.at(0)->GetTransform().Location - Vector3::Vec3ToVector(Graphics::MainCamera->Position)).Length() * 0.15f);
	if (UserDraggingButton)
	{
		UIBorder DragUIBorder(Vector2(Input::MouseLocation.X + 0.1f, Input::MouseLocation.Y + 0.1f), Vector2(-2.f / 16, -2.f / 9), Vector3(1),
			ContentBrowserMode ? Textures[2] : GetTextureFromFilePath(ContentBrowser::ContentAssets.at(DraggedButton).FilePath, Textures), true);
		DragUIBorder.Render(Shader);
		if (!ContentBrowserMode)
		{
			size_t lastindex = GetFileNameFromPath(ContentBrowser::ContentAssets.at(DraggedButton).FilePath).find_last_of(".");
			std::string rawname = GetFileNameFromPath(ContentBrowser::ContentAssets.at(DraggedButton).FilePath).substr(0, lastindex);
			EngineUITextRenderer.RenderText(rawname, Vector2(Input::MouseLocation.X - 0.015f, Input::MouseLocation.Y - 0.11f), 1.f, Vector3(1));
		}
		else
		{
			EngineUITextRenderer.RenderText(Objects::EditorObjects[DraggedButton].Name, Vector2(Input::MouseLocation.X - 0.015f, Input::MouseLocation.Y - 0.11f),
				1.f, Vector3(1));
		}
		if (!Input::IsLMBDown)
		{
			if (Maths::IsPointIn2DBox(Vector2(0.7f, 0.8f), Vector2(-0.7f, -0.6f), Input::MouseLocation))
			{
				if (ContentBrowserMode)
				{
					Collision::HitResponse CollisionTest = Collision::LineTrace(Vector3::Vec3ToVector(Graphics::MainCamera->Position),
						(Rotation * 500.f) + Vector3::Vec3ToVector(Graphics::MainCamera->Position));
					if (CollisionTest.Hit)
					{
						Objects::SpawnObjectFromID(Objects::EditorObjects[DraggedButton].ID,
							Transform(Vector3::SnapToGrid(CollisionTest.ImpactPoint, 0.5f), Vector3(0), Vector3(1.f)));
					}
					else
					{
						Objects::SpawnObjectFromID(Objects::EditorObjects[DraggedButton].ID,
							Transform(Vector3::SnapToGrid((Rotation * 45) + Vector3::Vec3ToVector(Graphics::MainCamera->Position), 0.5f), Vector3(0), Vector3(1.f)));
					}
					UpdateContentBrowser();
					ChangedScene = true;
				}
				else
				{
					std::string Ext = ContentBrowser::ContentAssets.at(DraggedButton).FilePath.substr(ContentBrowser::ContentAssets.at(DraggedButton).FilePath.find_last_of(".") + 1);
					if (Ext == "jsm")
					{
						Collision::HitResponse CollisionTest = Collision::LineTrace(Vector3::Vec3ToVector(Graphics::MainCamera->Position),
							(Rotation * 500.f) + Vector3::Vec3ToVector(Graphics::MainCamera->Position));
						if (CollisionTest.Hit)
						{
							Objects::CreateNewMeshObject(GetFileNameWithoutExtensionFromPath(ContentBrowser::ContentAssets.at(DraggedButton).FilePath),
								Transform(Vector3::SnapToGrid(CollisionTest.ImpactPoint, 0.5f), Vector3(0), Vector3(1.f)),
								GetFileNameWithoutExtensionFromPath(GetFileNameFromPath(ContentBrowser::ContentAssets.at(DraggedButton).FilePath)));
						}
						else
						{
							Objects::CreateNewMeshObject(GetFileNameWithoutExtensionFromPath(ContentBrowser::ContentAssets.at(DraggedButton).FilePath),
								Transform(Vector3::SnapToGrid((Rotation * 45.f) + Vector3::Vec3ToVector(Graphics::MainCamera->Position), 0.5f), Vector3(0),
									Vector3(1.f)), GetFileNameWithoutExtensionFromPath(GetFileNameFromPath(ContentBrowser::ContentAssets.at(DraggedButton).FilePath)));
						}
						UpdateContentBrowser();
						ChangedScene = true;
					}
				}
				UserDraggingButton = false;
			}
		}
	}
	else if (Input::IsLMBDown && !LMBDown)
	{
		if (TabWidgets.at(EngineUIVariables::Tabs.at(SelectedTab).Type) == nullptr)
		{
			LMBDown = true;
			if (Maths::IsPointIn2DBox(Vector2(0.7f, 0.675f), Vector2(-0.7f, -0.6f), Input::MouseLocation))
			{
				bool Hit = false;
				if (SelectedObjects.size() > 0)
				{
					float t = INFINITY;
					Collision::HitResponse
						CollisionTest = Collision::LineCheckForAABB((ArrowBoxZ * DistanceScaleMultiplier) + SelectedObjects.at(0)->GetTransform().Location,
							Vector3::Vec3ToVector(Graphics::MainCamera->Position), (Rotation * 500.f) + Vector3::Vec3ToVector(Graphics::MainCamera->Position));
					if (CollisionTest.Hit)
					{
						PreviosLocation = Rotation;
						Hit = true;
						Dragging = true;
						Axis = Vector3(0, 0, 1.f);
						t = CollisionTest.t;

					}
					CollisionTest = Collision::LineCheckForAABB((ArrowBoxY * DistanceScaleMultiplier) + SelectedObjects.at(0)->GetTransform().Location,
						Vector3::Vec3ToVector(Graphics::MainCamera->Position), (Rotation * 500.f) + Vector3::Vec3ToVector(Graphics::MainCamera->Position));
					if (CollisionTest.Hit && CollisionTest.t < t)
					{
						PreviosLocation = Rotation;
						Hit = true;
						Dragging = true;
						Axis = Vector3(0, 1.f, 0);
						t = CollisionTest.t;
					}
					CollisionTest = Collision::LineCheckForAABB((ArrowBoxX * DistanceScaleMultiplier) + SelectedObjects.at(0)->GetTransform().Location,
						Vector3::Vec3ToVector(Graphics::MainCamera->Position), (Rotation * 500.f) + Vector3::Vec3ToVector(Graphics::MainCamera->Position));
					if (CollisionTest.Hit && CollisionTest.t < t)
					{
						PreviosLocation = Rotation;
						Hit = true;
						Dragging = true;
						Axis = Vector3(1.f, 0, 0);
						t = CollisionTest.t;
					}
				}

				Collision::HitResponse CollisionTest = Collision::LineTrace(Vector3::Vec3ToVector(Graphics::MainCamera->Position),
					(Rotation * 5000.f) + Vector3::Vec3ToVector(Graphics::MainCamera->Position));
				if (CollisionTest.Hit && !Hit)
				{
					for (int i = 0; i < Objects::AllObjects.size(); i++)
					{
						if (Objects::AllObjects[i] != CollisionTest.HitObject)
						Objects::AllObjects.at(i)->IsSelected = false;
					}
					CollisionTest.HitObject->IsSelected = true;
					ContextMenu->SetObject(CollisionTest.HitObject);
				}
			}
		}
	}
	else if(!Input::IsLMBDown)
	{
		LMBDown = false;
		Dragging = false;
	}

	if (Dragging && TabWidgets.at(EngineUIVariables::Tabs.at(SelectedTab).Type) == nullptr)
	{
		Vector3 TransformToAdd = ((Rotation * Axis) - (PreviosLocation * Axis)) * (Vector3(5) * DistanceScaleMultiplier);
		for (int i = 0; i < Objects::AllObjects.size(); i++)
		{
			if (Objects::AllObjects.at(i)->IsSelected)
			{
				Objects::AllObjects.at(i)->SetTransform(Objects::AllObjects.at(i)->GetTransform() + Transform(TransformToAdd, Vector3(), Vector3(1)));
			}
		}
		PreviosLocation = Rotation;
		ChangedScene = true;
		Input::CursorVisible = false;
	}
	else Input::CursorVisible = true;
	if (SelectedObjects.size() > 0)
	{
		
		if (Input::IsKeyDown(SDLK_LCTRL) && Input::IsKeyDown(SDLK_w) && !Input::IsRMBDown)
		{
			if (!Copied)
			{
				WorldObject* NewObject = Objects::SpawnObjectFromID(SelectedObjects[0]->GetObjectDescription().ID, SelectedObjects[0]->GetTransform());
				NewObject->Deserialize(SelectedObjects[0]->Serialize());
				NewObject->LoadProperties(SelectedObjects[0]->GetPropertiesAsString());
				NewObject->SetName(SelectedObjects[0]->GetName());
				for (int i = 0; i < Objects::AllObjects.size(); i++)
				{
					Objects::AllObjects.at(i)->IsSelected = false;
				}
				NewObject->IsSelected = true;
				ContextMenu->SetObject(NewObject);
				DoSafeUpdate = true;
				Copied = true;
				ChangedScene = true;
			}
		}
		else Copied = false;
	}
	if (DoSafeUpdate)
	{
		ContentBrowser::UpdateContentAssets();
		UpdateContentBrowser();
		DoSafeUpdate = false;
	}
	if (ShouldStopDragging)
	{
		UserDraggingButton = false;
		ShouldStopDragging = false;
	}
	if (!Input::IsLMBDown)
	{
		if (UserDraggingButton)
		{
			ShouldStopDragging = true;
		}
	}
	else
	{
		ShouldStopDragging = false;
	}

	if (PopUpWindow)
	{
		Input::MouseLocation = PrevMousePos;
		for (UIBorder* b : PopUpUI)
		{
			b->Render(Shader);
		}
		EngineUITextRenderer.RenderText(PopUpMesage, Vector2(-0.2, -0.075f), 2, Vector3(1));
		for (uint32_t i = 0; i < PopUpButtons.size(); i++)
		{
			EngineUITextRenderer.RenderText(PopUpButtons[i].Text, Vector2(-0.18 + (i / 7.f), -0.18), 1.5, Vector3(0));
		}
	}
}


//Content Browser

void EngineUI::UpdateContentBrowser()
{
	RenamingContentAsset = false;
	RenamingWorldObject = false;
	Debugging::EngineStatus = "EngineUIRendering: UpdateContentBrowser";
	GenUITextures();
	for (int i = 0; i < ContentButtons.size(); i++)
	{
		delete ContentButtons[i];
	}
	ContentButtons.clear();
	int j = 0;
	if (!ContentBrowserMode)
	{
		for (int i = 0; i < ContentBrowser::ContentAssets.size(); i++)
		{
			try
			{
				UIButton* NewUIButton = new UIButton
				(
					Vector2(-0.86f + ((j % 2) / 7.f), 0.7f - ((int)(j / 2) / 4.f)),
					Vector2(-2.f / 16, -2.f / 9),
					Vector3(1),
					this, i, true,
					GetTextureFromFilePath(ContentBrowser::ContentAssets[i].FilePath, Textures),
					true
				);
				NewUIButton->CurrentScrollObject = &ContentBrowserScrollObject;
				NewUIButton->NeedsToBeSelected = true;
				ContentButtons.push_back(NewUIButton);
				j++;
			}
			catch (std::exception)
			{ }
		}
	}
	else
	{
		for (int i = 0; i < Objects::EditorObjects.size(); i++)
		{
			try
			{
				UIButton* NewUIButton = new UIButton
				(
					Vector2(-0.86f + ((j % 2) / 7.f), 0.7f - ((int)(j / 2) / 4.f)),
					Vector2(-2.f / 16, -2.f / 9),
					Vector3(1),
					this, i, true,
					Textures[2],
					true
				);
				NewUIButton->CurrentScrollObject = &ContentBrowserScrollObject;
				NewUIButton->NeedsToBeSelected = true;
				ContentButtons.push_back(NewUIButton);
				j++;
			}
			catch (std::exception& e)
			{
				Log::CreateNewLogMessage(e.what(), Vector3(1, 0, 0));
			}
		}
	}
	for (UIBorder* b : WorldButtons)
	{
		delete b;
	}
	WorldButtons.clear();
	for (int i = 0; i < Objects::AllObjects.size(); i++)
	{
		WorldButtons.push_back(new UIButton(Vector2(0.96f, 0.7f - (i / 16.f)),
			Vector2(-0.2f, -0.05f),
			Color,
			this, i + 200, false));
		WorldButtons.at(WorldButtons.size() - 1)->CurrentScrollObject = &WorldObjectsScrollObject;
	}
	SelectedTab = SelectedTab >= EngineUIVariables::Tabs.size() ? 0 : SelectedTab;
	for (UIBorder* b : TabButtons)
	{
		delete b;
	}
	TabButtons.clear();
	for (int i = 0; i < EngineUIVariables::Tabs.size(); i++)
	{
		TabButtons.push_back(new UIButton(Vector2(-0.7f + ((float)i / 5), 0.68f),
			Vector2(0.15f, 0.05f), SelectedTab == i ? Vector3(0.2f, 0.2f, 0.22f) : Vector3(0.1f, 0.1f, 0.125f), this, i * 2 - 300, false));
		if (EngineUIVariables::Tabs.at(i).CanBeClosed)
		{
			TabButtons.push_back(new UIButton(Vector2(-0.7f + ((float)i / 5) + 0.15f, 0.68f),
				Vector2(0.05f * 0.5625, 0.05f), Vector3(1), this, i * 2 - 299, false, Textures.at(11), true, true));
		}
		else
		{
			TabButtons.push_back(new UIBorder(Vector2(-0.7f + ((float)i / 5) + 0.15f, 0.68f),
				Vector2(0.05f * 0.5625, 0.05f), SelectedTab == i ? Vector3(0.2f, 0.2f, 0.22f) : Vector3(0.1f, 0.1f, 0.125f)));
		}
	}
	AddItemsButtonsPressed.clear();
	for (int i = 0; i < 3; i++)
	{
		AddItemsButtonsPressed.push_back(false);
	}
}

void EngineUI::GenUITextures()
{
	const int ImageSize = 16;
	std::string Images[ImageSize]
	{												//Texture Indices
		"EngineContent/Images/Model.png",			//00 -> Model Content Browser Symbol
		"EngineContent/Images/Folder.png",			//01 -> Folder Content Browser Symbol
		"EngineContent/Images/Script.png",			//02 -> Script..? Content Browser Symbol
		"EngineContent/Images/Unkown.png",			//03 -> "Unknown" Content Browser Symbol
		"EngineContent/Images/Text.png",			//04 -> Text File Content Browser Symbol? (Why?)
		"EngineContent/Images/Arrow.png",			//05 -> Content Browser Back Arrow
		"EngineContent/Images/Image.png",			//06 -> Image (png file) Content Browser Symbol
		"EngineContent/Images/Save.png",			//07 -> Save Button
		"EngineContent/Images/Build.png",			//08 -> Build Project
		"EngineContent/Images/Recompile.png",		//09 -> Recompile... also something (also unused)
		"EngineContent/Images/Level.png",			//10 -> Scene Content Browser Symbol
		"EngineContent/Images/X.png",				//11 -> X Symbol for Tab
		"EngineContent/Images/Material.png",		//12 -> Material Content Browser Symbol
		"EngineContent/Images/MaterialTemplate.png",//13 -> Material Template Content Browser Symbol
		"EngineContent/Images/SubLevel.png",		//14 -> SubScene Content Browser Symbol
		"EngineContent/Images/Sound.png",			//15 -> Sound Content Browser Symbol
	};

	for (int i = 0; i < Textures.size(); i++)
	{
		glDeleteTextures(1, &Textures.at(i));
	}
	for (int i = 0; i < ImageSize; i++)
	{
		int TextureWidth = 0;
		int TextureHeigth = 0;
		int BitsPerPixel = 0;
		stbi_set_flip_vertically_on_load(true);
		auto TextureBuffer = stbi_load(Images[i].c_str(), &TextureWidth, &TextureHeigth, &BitsPerPixel, 4);


		GLuint TextureID;
		glGenTextures(1, &TextureID);
		glBindTexture(GL_TEXTURE_2D, TextureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TextureWidth, TextureHeigth, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureBuffer);

		Textures.push_back(TextureID);
		if (TextureBuffer)
		{
			stbi_image_free(TextureBuffer);
		}
	}
}

UIButton* EngineUI::CreateNewButton(Vector2 Position, Vector2 Scale, Vector3 Color, int Index)
{
	UIButton* NewButton = new UIButton(Position, Scale, Color, this, Index, false);
	UIElements.push_back(NewButton);
	return NewButton;
}