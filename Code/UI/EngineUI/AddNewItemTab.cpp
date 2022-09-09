#include "AddNewItemTab.h"
#include <fstream>
#include "EngineUI.h"
#include <WorldParameters.h>
#include <FileUtility.h>

std::string GetTypeNameFromExtension(std::string Extension)
{
	if (Extension == "jsmat")
	{
		return "Material";
	}
	if (Extension == "jsmtmp")
	{
		return "Material Template";
	}
	if (Extension == "cpp" || Extension == "hpp" || Extension == "h")
	{
		return "C++ Source";
	}
	if (Extension == "txt")
	{
		return "Text File";
	}
	if (Extension == "jscn")
	{
		return "Scene";
	}
	if (Extension == "subscn")
	{
		return "Subscene";
	}
	return "Unknown File";
}

void AddNewItemTab::Render(Shader* Shader)
{
	for (UIBorder* b : UIElements)
	{
		b->Render(Shader);
	}
	Renderer->RenderText("Create", Vector2(0.355f, -0.03f), 2, Vector3(0));
	Renderer->RenderText("Current File = " + ContentBrowser::CurrentFilePath + "/" + MainTextField->Text, Vector2(-0.3f, 0.15f), 1, Vector3(1));
	Renderer->RenderText("Current File Type = " + GetTypeNameFromExtension(GetExtension(MainTextField->Text)), Vector2(-0.3f, 0.1f), 1, Vector3(1));
}

AddNewItemTab::AddNewItemTab(TextRenderer* Renderer, EngineUI* Parent) : EngineTab(Vector2(), Vector2(), Vector3())
{
	this->Renderer = Renderer;
	this->ParentUI = Parent;
	UIElements.push_back(new UIBorder(Vector2(-0.7f, -0.6f), Vector2(1.4, 1.28f), Vector3(0.1f, 0.1f, 0.105f)));
	MainTextField = new TextField(Vector2(-0.3f, -0.05f), Vector2(0.6f, 0.1f), Vector3(0.2f), Renderer, this, 0, 1.5f);
	UIElements.push_back(MainTextField);
	UIElements.push_back(new UIButton(Vector2(0.35f, -0.05f), Vector2(0.125f, 0.1f), Vector3(0.1f, 0.7f, 0.1f), this, 1));
}

AddNewItemTab::~AddNewItemTab()
{
}

void AddNewItemTab::Load(std::string Filepath)
{
	MainTextField->Text = "[Enter File Name here]";
	FileToAdd = "";
}

void AddNewItemTab::Save()
{
}

void AddNewItemTab::OnButtonClicked(int Index)
{
	switch (Index)
	{
	case 0:
		FileToAdd = ContentBrowser::CurrentFilePath + "/" + MainTextField->Text;
		break;
	case 1:
		if (FileToAdd != "")
		{
			std::ofstream Out = std::ofstream(FileToAdd, std::ios::out);
			Out.close();
			ParentUI->DoSafeUpdate = true;
		}
		break;
	}
}