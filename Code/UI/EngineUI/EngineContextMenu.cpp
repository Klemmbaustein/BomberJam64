#include "EngineContextMenu.h"
#include <Input.h>
#include <glm/glm.hpp>
#include <FileUtility.h>
#include <UI/EngineUI/EngineUI.h>

bool ShouldUpdate = false;

void EngineContextMenu::OnButtonClicked(int Index)
{
	switch (Index)
	{
	case 1:
	{
		CurrentObject->SetTransform(Transform(TransformTextFields.at(0)->GetValue(), CurrentObject->GetTransform().Rotation, CurrentObject->GetTransform().Scale));
		LastTransform = CurrentObject->GetTransform();
		TransformTextFields.at(Index - 1)->SetValue(CurrentObject->GetTransform().Location);
		break;
	}
	case 2:
	{
		CurrentObject->SetTransform(Transform(CurrentObject->GetTransform().Location, TransformTextFields.at(1)->GetValue().DegreesToRadiants(), CurrentObject->GetTransform().Scale));
		LastTransform = CurrentObject->GetTransform();
		TransformTextFields.at(Index - 1)->SetValue(CurrentObject->GetTransform().Rotation.RadiantsToDegrees());
		break;
	}
	case 3:
	{
		CurrentObject->SetTransform(Transform(CurrentObject->GetTransform().Location, CurrentObject->GetTransform().Rotation, TransformTextFields.at(2)->GetValue()));
		LastTransform = CurrentObject->GetTransform();
		TransformTextFields.at(Index - 1)->SetValue(CurrentObject->GetTransform().Scale);
		break;
	}
	default:
		break;
	}
	if (Index > 3)
	{
		if (CurrentObject)
		{
			TypeEnum Type = CurrentObject->Properties[Index - 4].Type;
			switch(Type)
			{
			case T_FLOAT:
				*((float*)CurrentObject->Properties[Index - 4].Data) = std::stof(((TextField*)Properties[Index - 4])->Text);
				break;
			case T_INT:
				*((int*)CurrentObject->Properties[Index - 4].Data) = std::stoi(((TextField*)Properties[Index - 4])->Text);
				break;
			case T_STRING:
				*((std::string*)CurrentObject->Properties[Index - 4].Data) = ((TextField*)Properties[Index - 4])->Text;
				break;
			case T_VECTOR3:
				*((Vector3*)CurrentObject->Properties[Index - 4].Data) = ((VectorInputField*)Properties[Index - 4])->GetValue();
				break;
			default:
				break;
			}
			CurrentObject->OnPropertySet();
			ChangedScene = true;
		}
		ShouldUpdate = true;
	}
}

void EngineContextMenu::Render(Shader* Shader)
{
	try
	{
		if (CurrentObject != nullptr)
		{
			Renderer->RenderText("Context Menu: ", Vector2(0.72, -0.3), 1.3f, Vector3(1), &ContextMenuScrollObject);
			Renderer->RenderText("\"" + CurrentObject->GetName() + "\" in scene " + GetFileNameWithoutExtensionFromPath(CurrentObject->CurrentScene),
				Vector2(0.72, -0.335), 0.8f, Vector3(1), &ContextMenuScrollObject);
			Renderer->RenderText("Transform: ", Vector2(0.72, -0.37), 1.f, Vector3(1), &ContextMenuScrollObject);
			if (LastTransform != CurrentObject->GetTransform())
			{
				TransformTextFields.at(0)->SetValue(CurrentObject->GetTransform().Location);
				TransformTextFields.at(1)->SetValue(CurrentObject->GetTransform().Rotation.RadiantsToDegrees());
				TransformTextFields.at(2)->SetValue(CurrentObject->GetTransform().Scale);
			}
			for (UIBorder* b : UIElements)
			{
				b->Render(Shader);
			}
			uint16_t i = 0;
			for (UIBorder* b : Properties)
			{
				Renderer->RenderText(CurrentObject->Properties[i].Name, TextLocations[i] + Vector2(0, 0.1f), 1.2f, Vector3(1), &ContextMenuScrollObject);
				b->Render(Shader);
				i++;
			}
			LastTransform = CurrentObject->GetTransform();
			if (!CurrentObject->IsSelected)
			{
				CurrentObject = nullptr;
			}
			if (ShouldUpdate)
			{
				Generate();
				ShouldUpdate = false;
			}
		}
		else
		{
			Renderer->RenderText("No Object Selected", Vector2(0.75, -0.35), 1.f, Vector3(1));
		}
		if (Input::IsKeyDown(SDLK_DELETE))
			CurrentObject = nullptr;
	}
	catch (std::exception& e)
	{
	}
}

EngineContextMenu::~EngineContextMenu()
{
	
}

void EngineContextMenu::SetObject(WorldObject* Object)
{
	CurrentObject = Object;
	Generate();
}

void EngineContextMenu::Generate()
{
	for (UIBorder* b : Properties)
	{
		delete b;
	}
	Properties.clear();
	TextLocations.clear();
	unsigned int i = 0;
	if (CurrentObject != nullptr)
	{
		for (Object::Property p : CurrentObject->Properties)
		{
			TextField* NewTextField = new TextField(Vector2(0.7, -0.9 - (float)i / 7.f), Vector2(0.15, 0.1), Vector3(0.2), Renderer, this, i + 4);
			switch (p.Type)
			{
			case T_FLOAT:
				NewTextField->Text = std::to_string(*(float*)p.Data);
				break;
			case T_INT:
				NewTextField->Text = std::to_string(*((int*)p.Data));
				break;
			case T_STRING:
				NewTextField->Text = *((std::string*)p.Data);
				break;
			case T_VECTOR3:
				delete NewTextField;
				NewTextField = (TextField*)new VectorInputField(Vector2(0.7, -0.9 - (float)i / 7.f), i + 4, Renderer, this, *(Vector3*)p.Data);
				break;
			default:
				break;
			}
			TextLocations.push_back(Vector2(0.72, -0.89 - (float)i / 7.f));
			CurrentObject->Properties[i].Data;
			Properties.push_back(NewTextField);
			i++;
		}
		for (UIBorder* b : Properties)
		{
			b->CurrentScrollObject = &ContextMenuScrollObject;
		}
	}
}