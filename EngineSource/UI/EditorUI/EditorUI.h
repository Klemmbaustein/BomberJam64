#pragma once
#include "UI/Default/UICanvas.h"
#include "UI/Default/TextRenderer.h"
#include "UI/Default/UIButton.h"
#include <UI/Default/TextField.h>
#include <UI/EditorUI/MaterialTemplate.h>
#include <UI/EditorUI/EditorTab.h>
#include "Material.h"
#include "EditorContextMenu.h"
#include "MeshTab.h"
#include "AddNewItemTab.h"
#include "ParticleEditorTab.h"
#include <UI/Default/ScrollObject.h>

#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>
#include <thread>

class EngineUI;
extern bool ChangedScene;

namespace ContentBrowser
{

	struct Asset
	{
		std::string FilePath;
		bool IsDirectory = false;
	};

	extern std::string CurrentFilePath;

	extern std::vector<Asset> ContentAssets;
	void UpdateContentAssets();
}

namespace EngineUIVariables
{
	struct Tab
	{
		Tab(int Type, std::string Name, std::string Path, bool CanBeClosed = true)
		{
			this->Type = Type;
			this->Name = Name;
			this->CanBeClosed = CanBeClosed;
			this->Path = Path;

		}
		int Type;
		std::string Name;
		bool CanBeClosed;
		std::string Path;
	};
	extern std::vector<Tab> Tabs;
}

struct PopUpButton
{
	PopUpButton(std::string Text, bool CallFunc, void(EngineUI::*FPointer)())
	{
		this->Text = Text;
		this->CallFunc = CallFunc;
		this->FPointer = FPointer;
	}
	std::string Text = "PopUp";
	bool CallFunc = true;
	void(EngineUI::*FPointer)() = nullptr;
};

class EngineUI : public UICanvas
{
public:
	EngineUI();

	void OnLeave(void(*ReturnF)());
	void ShowPopUpWindow(std::string Message, std::vector<PopUpButton> Buttons);
	virtual void OnButtonClicked(int Index) override;
	virtual void Render(Shader* Shader) override; //Render and Update
	virtual void OnButtonDragged(int Index) override;
	void UpdateContentBrowser();
	void GenUITextures();


	UIButton* CreateNewButton(Vector2 Position, Vector2 Scale, Vector3 Color, int Index);

protected:
	std::vector<UIBorder*> PopUpUI;
	std::vector<PopUpButton> PopUpButtons;
	std::string PopUpMesage;
	bool PopUpWindow = false;
	std::thread* BuildThread = nullptr;
	TextRenderer EngineUITextRenderer;
	std::vector<UIButton*> ContentButtons;
	std::vector<bool> ContentButtonsPressed;
	std::vector<bool> ContentButtonsSelected;
	std::vector<UIButton*> WorldButtons;
	std::vector<UIBorder*> TabButtons;
	std::vector<bool> AddItemsButtonsPressed;
	std::vector<GLuint> Textures;
	std::vector<EngineTab*> TabWidgets; //Note to self: this is not unreal.
	EngineContextMenu* ContextMenu;
	bool DoSafeUpdate = false;
	bool LMBDown = false;
	bool ShouldStopDragging = false;
	bool RenamingContentAsset = false;
	bool RenamingWorldObject = false;
	int RenamedIndex = 0;
	ScrollObject WorldObjectsScrollObject = ScrollObject(Vector2(1.f, 0.8f), Vector2(0.3f, 1.f), 50);
	ScrollObject ContentBrowserScrollObject = ScrollObject(Vector2(-0.7f, 0.7f), Vector2(0.3f, 1.3f), 15);
	ScrollObject LogScrollObject = ScrollObject(Vector2(0.675f, -0.63f), Vector2(1.375f, 0.3f), 50);
	bool AddNewItemDropDown = false;
	bool ContentBrowserMode = false;
	int DraggedButton = 0;
	SDL_Cursor* CrossCursor;
	SDL_Cursor* GrabCursor;
	SDL_Cursor* DefaultCursor;
	TextField* ConsoleTextField;
	friend AddNewItemTab;
	void Leave();
	void SaveChangesAndLeave();
};