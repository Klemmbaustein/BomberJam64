#pragma once
#include "Material.h"
#include "MaterialFunctions.h"
#include <fstream>
#include <filesystem>
#include <Log.h>
namespace fs = std::filesystem;

bool Material_DoSafeUpdate = false;

void MaterialUI::OnButtonClicked(int Index)
{
	if (Index == -2)
	{
		if (fs::exists(Assets::GetAsset(ShaderFileTextFields.at(0)->Text + ".jsmtmp")))
		{
			ParentTemplate = ShaderFileTextFields.at(0)->Text;
			LoadTemplate(ParentTemplate + ".jsmtmp");
		}
	}
	else if (Index == -3)
	{
		Save();
	}
	else if (Index == -4)
	{
		FetchTemplate(ParentTemplate);
		Log::CreateNewLogMessage("Fetched Template " + ParentTemplate + " for File " + Filepath);
	}
	else if (Index == -5)
	{
		IsTransparent = !IsTransparent;
	}
	else if (Index >= 0)
	{
		if (Paramters.at(Index).Type != 3)
		{
			Paramters.at(Index).Value = ((TextField*)TextFields.at(Index))->Text;
		}
		else
		{
			Paramters.at(Index).Value = ((VectorInputField*)TextFields.at(Index))->GetValue().ToString();
		}
	}
	Material_DoSafeUpdate = true;
}

void MaterialUI::Render(Shader* Shader)
{
	Shader->Bind();
	for (int i = 0; i < UIElements.size(); i++)
	{
		UIElements.at(i)->Render(Shader);
	}
	Renderer->RenderText("Material ", Vector2(-0.6f, 0.6f), 2.f, Vector3(1));
	Renderer->RenderText("Source Files:", Vector2(0.5f, 0.5f), 1.2f, Vector3(0.9));
	Renderer->RenderText(VertexShader, Vector2(0.5f, 0.45f), 1.f, Vector3(0.7));
	Renderer->RenderText(FragmentShader, Vector2(0.5f, 0.4f), 1.f, Vector3(0.7));

	IsTransparentButton->Color = IsTransparent ? Vector3(0.2, 0.8, 0.2) : Vector3(0.8, 0.2, 0.2);

	Renderer->RenderText("Parent Material Template:", Vector2(-0.6f, 0.525f), 1.2f, Vector3(0.9));
	Renderer->RenderText("Fetch Template", Vector2(0.35, 0.525), 0.8f, Vector3(1));
	Renderer->RenderText("New uniform", Vector2(-0.495f, 0.3f), 1.f, Vector3(0.1f));
	Renderer->RenderText("Uniforms", Vector2(-0.6f, 0.3f), 1.2f, Vector3(0.9));
	Renderer->RenderText("Uniform Name", Vector2(-0.6f, 0.225f), 1.f, Vector3(0.9));
	Renderer->RenderText("Uniform Type", Vector2(-0.15f, 0.225f), 1.f, Vector3(0.9));
	Renderer->RenderText("Uniform Value", Vector2(0.f, 0.225f), 1.f, Vector3(0.9));
	Renderer->RenderText("Is Transparent: " + std::string(IsTransparent ? "true" : "false"), Vector2(0.f, 0.4f), 1.f, Vector3(0.9));

	for (int i = 0; i < Paramters.size(); i++)
	{
		Renderer->RenderText(Paramters.at(i).UniformName, Vector2(-0.6f, 0.15f + i / -12.f), 1.f, Vector3(1.f));
		Renderer->RenderText(MaterialUniformTypeToString(Paramters.at(i).Type), Vector2(-0.15f, 0.15f + i / -12.f), 1.f, Vector3(1.f));

	}

	for (int i = 0; i < TextFields.size(); i++)
	{
		TextFields.at(i)->Render(Shader);
	}
	if (Material_DoSafeUpdate)
	{
		GenerateUI();
		Material_DoSafeUpdate = false;
	}
}


void MaterialUI::Load(std::string File)
{
	Filepath = File;
	Paramters.clear();
	try
	{
		std::ifstream in = std::ifstream(Filepath, std::ios::in);
		char ReadName[100];
		in.getline(ReadName, 100, '\n');
		ParentTemplate = ReadName;
		in.getline(ReadName, 100, '\n');
		VertexShader = ReadName;
		in.getline(ReadName, 100, '\n');
		FragmentShader = ReadName;
		in.getline(ReadName, 100, '\n');
		IsTransparent = ReadName[0] == '1' ? true : false;
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
				Paramters.push_back(CurrentParam);
			}
		}
		ShaderFileTextFields.at(0)->Text = ParentTemplate;
		in.close();
		Material_DoSafeUpdate = true;
	}
	catch (std::exception& e)
	{
		Log::CreateNewLogMessage(e.what(), Vector3(0.8f, 0.f, 0.f));
	}
}

void MaterialUI::LoadTemplate(std::string Template)
{
	Paramters.clear();
	Template = Assets::GetAsset(Template);
	try
	{
		std::ifstream in = std::ifstream(Template, std::ios::in);
		char VertexShaderName[100];
		in.getline(VertexShaderName, 100, '\n');
		char FragmentShaderName[100];
		in.getline(FragmentShaderName, 100, '\n');
		VertexShader = VertexShaderName;
		FragmentShader = FragmentShaderName;
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
				Paramters.push_back(CurrentParam);
			}
		}
		in.close();
		Material_DoSafeUpdate = true;
	}
	catch (std::exception& e)
	{
		Log::CreateNewLogMessage(e.what(), Vector3(0.8f, 0.f, 0.f));
	}
}

void MaterialUI::FetchTemplate(std::string Template)
{
	Template = Template + ".jsmtmp";
	Template = Assets::GetAsset(Template);
	if (fs::exists(Template))
	{
		std::vector<MaterialTemplates::TemplateParam> OldParams = Paramters;
		Paramters.clear();

		try
		{
			std::ifstream in = std::ifstream(Template, std::ios::in);
			char VertexShaderName[100];
			in.getline(VertexShaderName, 100, '\n');
			char FragmentShaderName[100];
			in.getline(FragmentShaderName, 100, '\n');
			VertexShader = VertexShaderName;
			FragmentShader = FragmentShaderName;
			int j = 0;
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
								if (OldParams.size() > j)
									CurrentParam.Value = OldParams.at(j).Value;
								else
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
						if (OldParams.size() > j)
							CurrentParam.Value = OldParams.at(j).Value;
						else
							CurrentParam.Value = CurrentValue;
						break;
					default:
						throw LoadError("Iterator is not not valid. are there semicolons in your name or value?");
					}
					CurrentValue.clear();
					ValueType++;
					Paramters.push_back(CurrentParam);
					j++;
				}
			}
			in.close();
			Material_DoSafeUpdate = true;
		}
		catch (std::exception& e)
		{
			Log::CreateNewLogMessage(e.what(), Vector3(0.8f, 0.f, 0.f));
		}
		Save();
	}
}

void MaterialUI::Save()
{
	std::ofstream out = std::ofstream(Filepath, std::ios::out);
	out << ParentTemplate << "\n";
	out << VertexShader << "\n" << FragmentShader << "\n";
	out << IsTransparent << "\n";
	for (MaterialTemplates::TemplateParam& p : Paramters)
	{
		out << p.UniformName << ";" << p.Type << ";" << p.Value << "\n";
	}
	out.close();
}


void MaterialUI::GenerateUI()
{
	for (int i = 0; i < TextFields.size(); i++)
	{
		delete TextFields.at(i);
	}
	TextFields.clear();
	for (int i = 0; i < Paramters.size(); i++)
	{
		if (Paramters.at(i).Type != 3)
		{
			TextFields.push_back(new TextField(Vector2(0.f, 0.13f + i / -12.f), Vector2(0.4f, 0.075f), Vector3(0.12f), Renderer, this, i));
			((TextField*)TextFields.at(TextFields.size() - 1))->Text = Paramters.at(i).Value;
		}
		else
		{
			TextFields.push_back(new VectorInputField(Vector2(0.f, 0.13f + i / -12.f), Vector2(0.4f, 0.075f), Vector3(0.12f), i, Renderer, this, Vector3::stov(Paramters.at(i).Value)));
		}
	}
}

