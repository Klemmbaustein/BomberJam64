#pragma once
#include "MaterialTemplate.h"
#include "MaterialFunctions.h"
#include <UI/Default/VectorInputField.h>
#include <fstream>
#include <Log.h>

bool DoSafeUpdate = false;

void MaterialTemplate::OnButtonClicked(int Index)
{
	if (Index == -1)
	{
		Parameters.push_back(MaterialTemplates::TemplateParam("Uniform name here", 0, "0"));
	}
	else if (Index == -2)
	{
		VertexShader = ShaderFileTextFields.at(0)->Text;
		FragmentShader = ShaderFileTextFields.at(1)->Text;
	}
	else if (Index == -3)
	{
		Save();
	}
	else if (Index >= 0)
	{
		switch(Index % 3)
		{
		case 0:
			Parameters.at(Index / 3).UniformName = ((TextField*)TextFields.at(Index))->Text;
			break;
		case 1:
			if (MaterialUniformTypeStringToInt(((TextField*)TextFields.at(Index))->Text) != -1)
			{
				Parameters.at((Index - 1) / 3).Type = MaterialUniformTypeStringToInt(((TextField*)TextFields.at(Index))->Text);
			}
			else
			{
				try
				{
					Parameters.at(std::floor((Index - 1) / 3)).Type = std::stoi(((TextField*)TextFields.at(Index))->Text);
				}
				catch (std::exception e)
				{
					Log::CreateNewLogMessage(std::string("Thrown Exeption: ") + e.what(), Vector3(1.f, 0.1f, 0.f));
					Log::CreateNewLogMessage("This probalby means you have entered a text that is not a valid data type (Or you broke something)", Vector3(1.f, 0.1f, 0.f));
				}
			}
			break;
		case 2:
			if (Parameters.at((Index - 1) / 3).Type != 3)
			{
				Parameters.at((Index - 2) / 3).Value = ((TextField*)TextFields.at(Index))->Text;
			}
			else
			{
				Parameters.at((Index - 2) / 3).Value = ((VectorInputField*)TextFields.at(Index))->GetValue().ToString();
			}
			break;
		}
	}
	else if (Index < -50 && Index >= -100)
	{
		Parameters.erase(Parameters.begin() + Index + 100);
		std::cout << Index + 100;
	}
	DoSafeUpdate = true;
}

void MaterialTemplate::Render(Shader* Shader)
{
	Shader->Bind();
	for (int i = 0; i < UIElements.size(); i++)
	{
		UIElements.at(i)->Render(Shader);
	}
	for (int i = 0; i < RemoveButtons.size(); i++)
	{
		if (RemoveButtons[i] != nullptr)
		{
			RemoveButtons.at(i)->Render(Shader); //For maximun confusion i will both use std::vector::at() and std::vector::operator[].
			Renderer->RenderText("Remove", RemoveButtons[i]->Position + Vector2(0.01f, 0.02f), 1.25f, Vector3());
		}
	}
	Renderer->RenderText("Material Template", Vector2(-0.6f, 0.625f), 1.5f, Vector3(1));

	Renderer->RenderText("Shader File", Vector2(-0.6f, 0.55f), 1.2f, Vector3(0.9));
	Renderer->RenderText("Vertex Shader (.vert)", Vector2(-0.6f, 0.5f), 1.f, Vector3(0.9));
	Renderer->RenderText("Fragment Shader (.frag)", Vector2(-0.1f, 0.5f), 1.f, Vector3(1));
	Renderer->RenderText("Save", Vector2(0.4, 0.525), 0.8f, Vector3(1));
	Renderer->RenderText("New uniform", Vector2(-0.495f, 0.3f), 1.f, Vector3(0.1f));
	Renderer->RenderText("Uniforms", Vector2(-0.6f, 0.3f), 1.2f, Vector3(0.9));
	Renderer->RenderText("Uniform Name", Vector2(-0.6f, 0.225f), 1.f, Vector3(0.9));
	Renderer->RenderText("Uniform Type", Vector2(-0.15f, 0.225f), 1.f, Vector3(0.9));
	Renderer->RenderText("Uniform Default Value", Vector2(0.f, 0.225f), 1.f, Vector3(0.9));

	for (int i = 0; i < TextFields.size(); i++)
	{
		TextFields.at(i)->Render(Shader);
	}
	if (DoSafeUpdate)
	{
		GenerateUI();
		DoSafeUpdate = false;
	}
}


void MaterialTemplate::Load(std::string File)
{
	Parameters.clear();
	Filepath = File;
	try
	{
		std::ifstream in = std::ifstream(Filepath, std::ios::in);
		
		char VertexShaderName[100];
		in.getline(VertexShaderName, 100, '\n');
		char FragmentShaderName[100];
		in.getline(FragmentShaderName, 100, '\n');
		VertexShader = VertexShaderName;
		FragmentShader = FragmentShaderName;
		ShaderFileTextFields.at(0)->Text = VertexShader;
		ShaderFileTextFields.at(1)->Text = FragmentShader;
		while (!in.eof())
		{
			char CurrentLine_c_str[100];
			std::string CurrentLine;
			in.getline(CurrentLine_c_str, 100, '\n');
			CurrentLine = CurrentLine_c_str;
			std::string CurrentValue;
			MaterialTemplates::TemplateParam CurrentParam = MaterialTemplates::TemplateParam("", 0, "");
			int ValueType = 0;
			if (CurrentLine.size() > 0)
			{

				for (int i = 0; i < CurrentLine.size(); i++)
				{
					if (CurrentLine.at(i) != ';')
						CurrentValue = CurrentValue + CurrentLine.at(i);
					else
					{
						switch (ValueType)
						{
						case 0:
							CurrentParam.UniformName = CurrentValue;
							break;
						case 1:
							CurrentParam.Type = std::stoi(CurrentValue);
							break;
						case 2:
							CurrentParam.Value = CurrentValue;
							break;
						default:
							throw LoadError("Iterator is not not valid. are there semicolons in your name or value?");
						}
						CurrentValue.clear();
						ValueType++;
					}
				}
				switch (ValueType)
				{
				case 0:
					CurrentParam.UniformName = CurrentValue;
					break;
				case 1:
					CurrentParam.Type = std::stoi(CurrentValue);
					break;
				case 2:
					CurrentParam.Value = CurrentValue;
					break;
				default:
					throw LoadError("Iterator is not not valid. are there semicolons in your name or value?");
				}
				CurrentValue.clear();
				ValueType++;
				Parameters.push_back(CurrentParam);
			}
		}
		in.close();
		DoSafeUpdate = true;
	}
	catch (std::exception& e)
	{
		Log::CreateNewLogMessage(e.what(), Vector3(0.8f, 0.f, 0.f));
	}

}

void MaterialTemplate::Save()
{
	std::ofstream out = std::ofstream(Filepath, std::ios::out);
	out << VertexShader << "\n" << FragmentShader << "\n";
	for (MaterialTemplates::TemplateParam& p : Parameters)
	{
		out << p.UniformName << ";" << p.Type << ";" << p.Value << "\n";
	}
	out.close();
}


void MaterialTemplate::GenerateUI()
{
	for (int i = 0; i < TextFields.size(); i++)
	{
		delete TextFields.at(i);
	}
	for (UIButton* b : RemoveButtons)
	{
		delete b;
	}
	RemoveButtons.clear();
	TextFields.clear();
	for (int i = 0; i < Parameters.size() * 3; i += 3)
	{
		TextFields.push_back(new TextField(Vector2(-0.6f, 0.13f + i / -36.f), Vector2(0.425f, 0.075f), Vector3(0.12f), Renderer, this, i));
		((TextField*)TextFields.at(TextFields.size() - 1))->Text = Parameters.at(i / 3).UniformName;
		TextFields.push_back(new TextField(Vector2(-0.15f, 0.13f + i / -36.f), Vector2(0.125f, 0.075f), Vector3(0.12f), Renderer, this, i + 1));
		((TextField*)TextFields.at(TextFields.size() - 1))->Text = MaterialUniformTypeToString(Parameters.at(i / 3).Type);
		if (Parameters.at(i / 3).Type != 3)
		{
			TextFields.push_back(new TextField(Vector2(0.f, 0.13f + i / -36.f), Vector2(0.31f, 0.075f), Vector3(0.12f), Renderer, this, i + 2));
			((TextField*)TextFields.at(TextFields.size() - 1))->Text = Parameters.at(i / 3).Value;
		}
		else
		{
			TextFields.push_back(new VectorInputField(Vector2(0.f, 0.13f + i / -36.f), Vector2(0.4f, 0.075f), Vector3(0.12f), i + 2, Renderer, this,
				Vector3::stov(Parameters.at(i / 3).Value)));
		}
	}
	for (int i = 0; i < Parameters.size(); i++)
	{
		RemoveButtons.push_back(new UIButton(Vector2(0.325f, 0.13f + i / -12.f), Vector2(0.1f, 0.075f), Vector3(0.7f, 0, 0), this, i - 100));
	}
}