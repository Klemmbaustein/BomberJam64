#pragma once
#include "MeshTab.h"
#include <fstream>
#include <Engine/FileUtility.h>
#include <Importers/ModelConverter.h>
#include <Rendering/Mesh/JSModel.h>
#include <Engine/Log.h>
namespace MaterialTemplates
{
	struct TemplateParam
	{
		std::string UniformName;
		int Type;
		std::string Value;
		TemplateParam(std::string UniformName, int Type, std::string Value)
		{
			this->UniformName = UniformName;
			this->Type = Type;
			this->Value = Value;
		}
	};
}
JSM::ModelData ModelData;
std::vector<std::vector<Vertex>> MeshVertices;
std::vector<std::vector<int>> Indices;
bool CastShadow;
bool HasCollision = true;
bool TwoSided = true;
int NumTotalVertices;
std::string MeshPath;
MeshTab::MeshTab(TextRenderer* Renderer) : EngineTab(Vector2(), Vector2(), Vector3())
{
	this->Renderer = Renderer;
	UIElements.push_back(new UIBorder(Vector2(-0.7f, -0.6f), Vector2(1.4, 1.28f), Vector3(0.1f, 0.1f, 0.105f)));
	UIElements.push_back(CastShadowButton);
	UIElements.push_back(HasCollisionButton);
	UIElements.push_back(TwoSidedButton);
}

void MeshTab::Render(Shader* Shader)
{
	try
	{
		Shader->Bind();
		for (int i = 0; i < UIElements.size(); i++)
		{
			UIElements.at(i)->Render(Shader);
		}
		for (int i = 0; i < Materials.size(); i++)
		{
			TextFields.at(i)->Render(Shader);
		}
		Renderer->RenderText("Material Slots:", Vector2(-0.2f, 0.5f), 1.5, Vector3(1));
		Renderer->RenderText("Mesh: " + GetFilePathWithoutExtension(Filepath), Vector2(-0.6f, 0.5f), 1.5, Vector3(1));
		Renderer->RenderText("CastShadows  = " + std::string(CastShadow ? "true" : "false"), Vector2(-0.6f, 0.4375f), 1.f, Vector3(1));
		CastShadowButton->Color = CastShadow ? Vector3(0.2, 0.8, 0.2) : Vector3(0.8, 0.2, 0.2);
		Renderer->RenderText("HasCollision = " + std::string(HasCollision ? "true" : "false"), Vector2(-0.6f, 0.38f), 1.f, Vector3(1));
		Renderer->RenderText("Two Sided    = " + std::string(TwoSided ? "true" : "false"), Vector2(-0.6f, 0.325f), 1.f, Vector3(1));
		TwoSidedButton->Color = TwoSided ? Vector3(0.2, 0.8, 0.2) : Vector3(0.8, 0.2, 0.2);
		HasCollisionButton->Color = HasCollision ? Vector3(0.2, 0.8, 0.2) : Vector3(0.8, 0.2, 0.2);
		Renderer->RenderText("MaterialSlots = " + std::to_string(Materials.size()), Vector2(-0.6f, 0.27f), 1.f, Vector3(1));
		Renderer->RenderText("Vertices = " + std::to_string(NumTotalVertices), Vector2(-0.6f, 0.215f), 1.f, Vector3(1));
	}
	catch (std::exception& e)
	{
		Log::CreateNewLogMessage(e.what());
		throw(UIRenderingException("MeshTab", e.what()));
	}
}

void MeshTab::Load(std::string File)
{
	try
	{
		ModelData = JSM::LoadJSModel(File);
		for (std::string& m : ModelData.Materials)
		{
			m = m.substr(8);
		}
		NumTotalVertices = ModelData.NumVertices;
		HasCollision = ModelData.HasCollision;
		TwoSided = ModelData.TwoSided;
		CastShadow = ModelData.CastShadow;
		Materials = ModelData.Materials;
		MeshVertices = ModelData.Vertices;
		Indices = ModelData.Indices;
		InitialName = File;
	}
	catch (std::exception& e)
	{
		Log::CreateNewLogMessage(e.what());
	}

	Generate();
}

void MeshTab::ReloadMesh()
{
	Log::CreateNewLogMessage(InitialName);
	try
	{
		ModelData = JSM::LoadJSModel(InitialName);
		NumTotalVertices = ModelData.NumVertices;
		HasCollision = ModelData.HasCollision;
		TwoSided = ModelData.TwoSided;
		CastShadow = ModelData.CastShadow;
		Materials = ModelData.Materials;
		MeshVertices = ModelData.Vertices;
		Indices = ModelData.Indices;
	}
	catch (std::exception& e)
	{
		Log::CreateNewLogMessage(e.what());
	}

	Generate();
}

void MeshTab::Save()
{
	try
	{
		ModelData.Materials = Materials;
		ModelData.CastShadow = CastShadow;
		ModelData.HasCollision = HasCollision;
		ModelData.TwoSided = TwoSided;
		JSM::SaveJSModel(ModelData, InitialName);
	}
	catch (std::exception& e)
	{
		Log::CreateNewLogMessage(e.what());
	}
}

void MeshTab::Generate()
{
	TextFields.clear();
	for (int i = 0; i < Materials.size(); i++)
	{
		TextFields.push_back(new TextField(Vector2(-0.2f, 0.4f - ((float)i / 15)), Vector2(0.4f, 0.05f), Vector3(0.2f), Renderer, this, 1));
		TextFields.at(TextFields.size() - 1)->Text = Materials.at(i);
	}
}

void MeshTab::OnButtonClicked(int Index)
{
	switch (Index)
	{
	case 1:
		for (int i = 0; i < Materials.size(); i++)
		{
			Materials[i] = TextFields[i]->Text;
		}
		Generate();
		break;
	case 0:
		CastShadow = !CastShadow;
		break;
	case -2:
		HasCollision = !HasCollision;
		break;
	case -3:
		TwoSided = !TwoSided;
		break;
	default:
		break;
	}
}

MeshTab::~MeshTab()
{
	for (TextField* t : TextFields)
	{
		delete t;
	}
}
