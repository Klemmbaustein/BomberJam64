#include "ModelConverter.h"
#include "WorldParameters.h"
#include <filesystem>
#include <Log.h>
namespace fs = std::filesystem;

std::vector<ImportMesh> Meshes;
std::vector<Material> Materials;

void ProcessMesh(aiMesh* Mesh, const aiScene* Scene)
{
	ImportMesh m;
	for (Uint32 i = 0; i < Mesh->mNumVertices; i++)
	{
		m.Positions.push_back(Vector3(Mesh->mVertices[i].x, Mesh->mVertices[i].y, Mesh->mVertices[i].z));
		m.Normals.push_back(Vector3(Mesh->mNormals[i].x, Mesh->mNormals[i].y, Mesh->mNormals[i].z));
		if (Mesh->mTextureCoords[0])
			m.UVs.push_back(Vector2(Mesh->mTextureCoords[0][i].x, Mesh->mTextureCoords[0][i].y));
		else m.UVs.push_back(Vector2());
	}

	for (Uint32 i = 0; i < Mesh->mNumFaces; i++)
	{

		aiFace Face = Mesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		for (int j = 0; j < Face.mNumIndices; j++)
		{
			m.Indicies.push_back(Face.mIndices[j]);
		}
	}
	m.Material = Materials.at(Mesh->mMaterialIndex);
	Meshes.push_back(m);
}

void ProcessMaterials(const aiScene* Scene)
{
	if(Scene->mNumMaterials != 0)
	for (Uint32 i = 0; i < Scene->mNumMaterials; i++)
	{
		Material mat = {};
		aiMaterial* Material = Scene->mMaterials[i];
		aiColor3D Diffuse(1.f, 1.f, 1.f);
		if (AI_SUCCESS != Material->Get(AI_MATKEY_COLOR_DIFFUSE, Diffuse))
		{
			Log::CreateNewLogMessage("Error importing Mesh: No Diffuse Color found");
			// Error: No Diffuse Color
		}
		mat.Diffuse.X = Diffuse.r;
		mat.Diffuse.Y = Diffuse.g;
		mat.Diffuse.Z = Diffuse.b;

		aiColor3D SpecularScale;
		if (AI_SUCCESS != Material->Get(AI_MATKEY_COLOR_SPECULAR, SpecularScale))
		{
			Log::CreateNewLogMessage("Error importing Mesh: No Specular Color found");
		}


		aiColor3D Emissive(0.f, 0.f, 0.f);
		if (AI_SUCCESS != Material->Get(AI_MATKEY_COLOR_EMISSIVE, Emissive))
		{
			// Error: No Emissive Color
		}
		mat.Emissive.X = Emissive.r;
		mat.Emissive.Y = Emissive.g;
		mat.Emissive.Z = Emissive.b;
		float SpecularSize = 0.f;
		if (AI_SUCCESS != Material->Get(AI_MATKEY_SHININESS, SpecularSize))
		{
			//Error: No Shininess :(
		}
		mat.SpecularStrength = SpecularSize / 1;
		Materials.push_back(mat);
	}
	else
	{
		Material mat = {};
		mat.Diffuse = Vector3(1);
		mat.Emissive = Vector3(0);
		mat.SpecularSize = 0;
		mat.SpecularStrength = 0;
		Materials.push_back(mat);

	}
}

void ProcessNode(aiNode* Node, const aiScene* Scene)
{
	for (Uint32 i = 0; i < Node->mNumMeshes; i++)
	{
		aiMesh* Mesh = Scene->mMeshes[Node->mMeshes[i]];
		ProcessMesh(Mesh, Scene);
	}

	for (Uint32 i = 0; i < Node->mNumChildren; i++)
	{
		ProcessNode(Node->mChildren[i], Scene);
	}
}

std::string ModelImporter::Import(std::string Name, std::string CurrentFilepath)
{
	if (fs::exists(Name))
	{
		Meshes.clear();
		Materials.clear();
		Assimp::Importer Importer;
		const aiScene* Scene = Importer.ReadFile(Name, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_RemoveRedundantMaterials | aiProcess_PreTransformVertices);

		if (Scene == nullptr)
		{
			Log::CreateNewLogMessage(std::string("Error: Attempted to Import a non-3d Model as a 3d Model"), Vector3(1, 0.1f, 0.1f));
			return std::string("ERROR!");
		}

		if (!Scene || Scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
		{
			Log::CreateNewLogMessage("Error: Could not load Scene");
			if (Scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
				std::cout << ": AI_SCENE_FLAGS_INCOMPLETE";
			throw std::exception();
			return "";
		}
		ProcessMaterials(Scene);
		ProcessNode(Scene->mRootNode, Scene);

		std::string FileName = std::string(GetFileNameFromPath(Name));
		std::string FileNameWithoutExtension = FileName.substr(0, FileName.find_last_of("."));
		std::string OutputFileName = CurrentFilepath + "/" + FileNameWithoutExtension + ".jsm";
		
		{
			std::ofstream Output(OutputFileName, std::ios::out | std::ios::binary);
			int NumMaterials = Materials.size();
			Output.write((char*)&NumMaterials, sizeof(int));

			for (int j = 0; j < NumMaterials; j++)
			{
				ImportMesh CurrentMesh = Meshes.at(j);
				int NumVertices = CurrentMesh.Positions.size();
				int NumIndices = CurrentMesh.Indicies.size();

				Output.write((char*)&NumVertices, sizeof(int));
				Output.write((char*)&NumIndices, sizeof(int));


				for (int i = 0; i < NumVertices; i++)
				{
					Output.write((char*)&CurrentMesh.Positions.at(i).X, sizeof(float));
					Output.write((char*)&CurrentMesh.Positions.at(i).Y, sizeof(float));
					Output.write((char*)&CurrentMesh.Positions.at(i).Z, sizeof(float));
					Output.write((char*)&CurrentMesh.Normals.at(i).X, sizeof(float));
					Output.write((char*)&CurrentMesh.Normals.at(i).Y, sizeof(float));
					Output.write((char*)&CurrentMesh.Normals.at(i).Z, sizeof(float));
					Output.write((char*)&CurrentMesh.UVs.at(i).X, sizeof(float));
					Output.write((char*)&CurrentMesh.UVs.at(i).Y, sizeof(float));
				}
				for (int i = 0; i < NumIndices; i++)
				{
					Output.write((char*)&CurrentMesh.Indicies.at(i), sizeof(int));
				}
				std::string DefaultMaterial = "Content/NONE";
				size_t size = DefaultMaterial.size();

				Output.write((char*)&size, sizeof(size_t));
				Output.write(DefaultMaterial.c_str(), sizeof(char) * size);
			}
			bool CastShadow = true;
			bool TwoSided = false;
			Output.write((char*)&CastShadow, sizeof(uint8_t));
			Output.write((char*)&CastShadow, sizeof(uint8_t));
			Output.write((char*)&TwoSided, sizeof(uint8_t));
			Output.close();
			return OutputFileName;
		}
	}
	else Log::CreateNewLogMessage(Name + " does not exist");
	return "";
}