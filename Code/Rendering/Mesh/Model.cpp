#include "Model.h"
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include "Rendering/Camera/Camera.h"
#include <filesystem>
#include <Rendering/Utility/ShaderManager.h>
#include <Log.h>
#include <UI/EngineUI/MaterialFunctions.h>
#include <Rendering/Mesh/JSModel.h>
#include <World/Graphics.h>
#include <World/Stats.h>
#include <World/Assets.h>
namespace fs = std::filesystem;

const extern bool IsInEditor;
const extern bool EngineDebug;

Model::Model(std::string Filename)
{
	int NumVertecies = 0;
	int NumIndices = 0;

	uint64_t NumMeshes = 0;

	if (fs::exists(Filename))
	{
		JSM::ModelData ModelData = JSM::LoadJSModel(Filename);
		CastShadow = ModelData.CastShadow;
		TwoSided = ModelData.TwoSided;
		for (size_t i = 0; i < ModelData.Vertices.size(); i++)
		{
			for (size_t j = 0; j < ModelData.Vertices[i].size(); j++)
			{
				ModelMeshData.Vertices.push_back(ModelData.Vertices[i][j]);
			}
		}
		size_t prevSize = 0; 
		for (size_t i = 0; i < ModelData.Indices.size(); i++)
		{
			for (size_t j = 0; j < ModelData.Indices[i].size(); j++)
			{
				ModelMeshData.Indices.push_back(ModelData.Indices[i][j] + prevSize);
			}
			prevSize += ModelData.Vertices[i].size();
		}
		for (size_t i = 0; i < ModelData.Vertices.size(); i++)
		{
			Mesh* NewMesh = new Mesh(ModelData.Vertices[i], ModelData.Indices[i]);
			Meshes.push_back(NewMesh);
		}
		Materials = ModelData.Materials;
		HasCollision = ModelData.HasCollision;
		NonScaledSize = ModelData.CollisionBox.GetLength();
		LoadMaterials(Materials);
		ConfigureVAO();
	}
}


Model::~Model()
{
	for (Mesh* m : Meshes)
	{
		DereferenceShader("Shaders/" + m->VertexShader, "Shaders/" + m->FragmentShader);
		delete m;
	}
	Meshes.clear();
}
namespace CSM
{
	glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane);
	extern std::vector<unsigned int> ShadowMaps;
	const float cameraFarPlane = 700;
	extern std::vector<float> shadowCascadeLevels;
}

void Model::Render(Camera* WorldCamera)
{
	if (Visible)
	{
		if (Size.isOnFrustum(FrustumCulling::CurrentCameraFrustum, ModelTransform.Location, ModelTransform.Scale * NonScaledSize * 0.025))
		{
			if (TwoSided)
			{
				glDisable(GL_CULL_FACE);
			}
			else
			{
				glEnable(GL_CULL_FACE);
			}
			glm::mat4 ModelView;
			if (!Graphics::IsRenderingShadows)
			{
				ModelViewProjection = WorldCamera->getViewProj() * MatModel;
				ModelView = WorldCamera->getView() * MatModel;
			}
			glm::mat4 InvModelView = glm::transpose(glm::inverse(ModelView));
			glBindBuffer(GL_ARRAY_BUFFER, MatBuffer);
			for (int i = 0; i < Meshes.size(); i++)
			{
				Shader* CurrentShader = ((!Graphics::IsRenderingShadows || Meshes[i]->IsTransparent) ? Meshes.at(i)->MeshShader : Graphics::ShadowShader);
				CurrentShader->Bind();
				glUniformMatrix4fv(glGetUniformLocation(CurrentShader->GetShaderID(), "u_projection"), 1, GL_FALSE, &WorldCamera->GetProjection()[0][0]);
				glUniformMatrix4fv(glGetUniformLocation(CurrentShader->GetShaderID(), "u_invmodelview"), 1, GL_FALSE, &InvModelView[0][0]);
				glUniformMatrix4fv(glGetUniformLocation(CurrentShader->GetShaderID(), "u_viewpro"), 1, GL_FALSE, &WorldCamera->getViewProj()[0][0]);
				if (!Graphics::IsRenderingShadows)
					glUniformMatrix4fv(glGetUniformLocation(CurrentShader->GetShaderID(), "u_view"), 1, GL_FALSE, &WorldCamera->getView()[0][0]);
				Meshes.at(i)->Render(CurrentShader);
				Performance::DrawCalls++;
			}
		}
	}
}

void Model::LoadMaterials(std::vector<std::string> Materials)
{
	Materials.resize(Meshes.size());
	for (int j = 0; j < Meshes.size(); j++)
	{
		std::string m = Assets::GetAsset((Materials.at(j) + ".jsmat").substr(8));
		if (!fs::exists(m))
		{
			if(EngineDebug || IsInEditor)
				m = (Materials.at(j) + ".jsmat");
			else
				m = ("Assets/" + Materials.at(j) + ".jsmat");

			if (!fs::exists(m))
			{
				if (IsInEditor)
					Log::CreateNewLogMessage("Material given by the mesh does not exist. Falling back to default phong material", Vector3(1, 1, 0));
				m = "EngineContent/Materials/EngineDefaultPhong.jsmat";
			}
		}
		std::vector<MaterialUniforms::Param> Uniforms;

		try
		{
			if (!fs::exists(m))
			{
				return;
			}
			bool IsTransparent;
			std::ifstream in = std::ifstream(m, std::ios::in);
			char ReadName[100];
			in.getline(ReadName, 100, '\n');
			in.getline(ReadName, 100, '\n');
			Meshes.at(j)->VertexShader = ReadName;
			in.getline(ReadName, 100, '\n');
			Meshes.at(j)->FragmentShader = ReadName;
			in.getline(ReadName, 100, '\n');
			Meshes.at(j)->IsTransparent = ReadName[0] == '1' ? true : false;
			Meshes.at(j)->MeshShader = ReferenceShader("Shaders/" + Meshes.at(j)->VertexShader, "Shaders/" + Meshes.at(j)->FragmentShader);
			if (Meshes.at(j)->MeshShader == nullptr)
			{
				Meshes.at(j)->MeshShader = Graphics::MainShader;
				Log::CreateNewLogMessage("Invalid Shader");
			}
			while (!in.eof())
			{
				char CurrentLine_c_str[100];
				std::string CurrentLine;
				in.getline(CurrentLine_c_str, 100, '\n');
				CurrentLine = CurrentLine_c_str;

				std::string CurrentValue;
				MaterialUniforms::Param CurrentParam = MaterialUniforms::Param("", 0, "");
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
					Uniforms.push_back(CurrentParam);
				}
			}
		}
		catch (std::exception& e)
		{
			Log::CreateNewLogMessage(e.what(), Vector3(0.8f, 0.f, 0.f));
		}
		try
		{
			Meshes.at(j)->Uniforms.clear();
			for (int i = 0; i < Uniforms.size(); i++)
			{
				Meshes.at(j)->Uniforms.push_back(Uniform("", 0, nullptr));
				Meshes.at(j)->Uniforms.at(i).Name = Uniforms.at(i).UniformName;
				Meshes.at(j)->Uniforms.at(i).Value = Uniforms.at(i).Type;
				Meshes.at(j)->Uniforms.at(i).Content = (void*)Uniforms.at(i).Value.c_str();
			}
			Meshes.at(j)->ApplyUniforms();
		}
		catch (std::exception& e)
		{
			Log::CreateNewLogMessage(e.what());
		}
	}
}

void Model::ConfigureVAO()
{
	if (MatBuffer != -1)
		glDeleteBuffers(1, &MatBuffer);
	glGenBuffers(1, &MatBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, MatBuffer);
	glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(glm::mat4), &MatModel, GL_STATIC_DRAW);
	for (int i = 0; i < Meshes.size(); i++)
	{
		unsigned int VAO = Meshes[i]->MeshVertexBuffer->GetVAO();
		glBindVertexArray(VAO);
		// vertex attributes
		std::size_t vec4Size = sizeof(glm::vec4);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);

		glBindVertexArray(0);
	}
}

void Model::SimpleRender(Shader* Shader)
{
	if (Visible)
	{
		if (Size.isOnFrustum(FrustumCulling::CurrentCameraFrustum, ModelTransform.Location, ModelTransform.Scale * NonScaledSize * 0.025))
		{
			Shader->Bind();
			if (TwoSided)
			{
				glDisable(GL_CULL_FACE);
			}
			else
			{
				glEnable(GL_CULL_FACE);
			}
			glUniformMatrix4fv(glGetUniformLocation(Shader->GetShaderID(), "u_model"), 1, GL_FALSE, &MatModel[0][0]);
			for (Mesh* m : Meshes)
			{
				m->SimpleRender(Shader);
				Performance::DrawCalls++;
			}
		}
	}
}

void Model::SetUniform(Uniform NewUniform, uint8_t MeshIndex)
{
	size_t i = 0;
	for (auto& Mesh : Meshes[MeshIndex]->Uniforms)
	{
		if (Mesh.Name == NewUniform.Name)
		{
			Mesh.Value = NewUniform.Value;

			if (Mesh.Content != NewUniform.Content)
			{
				delete Mesh.Content;
				Mesh.Content = NewUniform.Content;
				Meshes[MeshIndex]->ApplyUniform(i);
			}
			return;
		}
		i++;
	}
	Meshes[MeshIndex]->Uniforms.push_back(NewUniform);
	Meshes[MeshIndex]->ApplyUniform(Meshes[MeshIndex]->Uniforms.size() - 1);
}
