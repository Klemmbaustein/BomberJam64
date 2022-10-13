#include "InstancedModel.h"
#include <filesystem>
#include <fstream>
#include <Rendering/Utility/ShaderManager.h>
#include <Rendering/Mesh/JSModel.h>
#include <Log.h>
#include <World/Graphics.h>
#include <World/Assets.h>
#include <World/Stats.h>

namespace fs = std::filesystem;


InstancedModel::InstancedModel(std::string Filename)
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
			InstancedMesh* NewMesh = new InstancedMesh(ModelData.Vertices[i], ModelData.Indices[i]);
			Meshes.push_back(NewMesh);
		}
		Materials = ModelData.Materials;
		if (ModelData.HasCollision)
		{
			NonScaledSize = ModelData.CollisionBox;
		}
		Uniforms.clear();

		LoadMaterials(Materials);

		ConfigureVAO();
	}
}

void InstancedModel::LoadMaterials(std::vector<std::string> Materials)
{
	Materials.resize(Meshes.size());
	for (int j = 0; j < Meshes.size(); j++)
	{
		std::string m = Assets::GetAsset((Materials.at(j) + ".jsmat").substr(8));
		if (!fs::exists(m))
		{
			if (EngineDebug || IsInEditor)
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

InstancedModel::~InstancedModel()
{
	for (InstancedMesh* m : Meshes)
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

void InstancedModel::Render(Camera* WorldCamera)
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
		ModelView = WorldCamera->getView() * MatModel[0];
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

size_t InstancedModel::AddInstance(Transform T)
{
	Instances.push_back(T);
	return Instances.size() - 1;
}

void InstancedModel::ConfigureVAO()
{
	MatModel.clear();
	for (Transform &T : Instances)
	{
		glm::mat4 Inst = glm::mat4(1.f);

		Size = (NonScaledSize * T.Scale * 0.025f);
		Inst = glm::translate(Inst, (glm::vec3)T.Location);
		Inst = glm::rotate(Inst, T.Rotation.Y, glm::vec3(0, 1, 0));
		Inst = glm::rotate(Inst, T.Rotation.Z, glm::vec3(0, 0, 1));
		Inst = glm::rotate(Inst, T.Rotation.X, glm::vec3(1, 0, 0));

		MatModel.push_back(glm::scale(Inst, (glm::vec3)(T.Scale) * 0.025f));
	}
	if(MatBuffer != -1)
		glDeleteBuffers(1, &MatBuffer);
	glGenBuffers(1, &MatBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, MatBuffer);
	glBufferData(GL_ARRAY_BUFFER, MatModel.size() * sizeof(glm::mat4), &MatModel[0], GL_STATIC_DRAW);
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
		Meshes[i]->SetInstances(Instances);
	}
}

void InstancedModel::SimpleRender(Shader* Shader)
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
	glBindBuffer(GL_ARRAY_BUFFER, MatBuffer);
	for (InstancedMesh* m : Meshes)
	{
		m->SimpleRender(Shader);
	}
	Performance::DrawCalls++;
}