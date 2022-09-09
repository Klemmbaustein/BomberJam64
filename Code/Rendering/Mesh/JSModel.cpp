#include "JSModel.h"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace JSM
{
	ModelData LoadJSModel(std::string Path)
	{
		if (std::filesystem::exists(Path))
		{
			std::ifstream Input = std::ifstream(Path, std::ios::in | std::ios::binary);

			ModelData ReadMesh;
			uint32_t NumMeshes, NumVertices, NumIndices;
			Input.read((char*)&NumMeshes, sizeof(int));

			for (int j = 0; j < NumMeshes; j++)
			{
				std::vector<Vertex> Vertices;
				std::vector<int> Indices;
				Input.read((char*)&NumVertices, sizeof(int));
				Input.read((char*)&NumIndices, sizeof(int));

				//read verts

				for (int i = 0; i < NumVertices; i++)
				{
					Vertex vertex;
					Input.read((char*)&vertex.Position.x, sizeof(float));
					Input.read((char*)&vertex.Position.y, sizeof(float));
					Input.read((char*)&vertex.Position.z, sizeof(float));
					Input.read((char*)&vertex.Normal.x, sizeof(float));
					Input.read((char*)&vertex.Normal.y, sizeof(float));
					Input.read((char*)&vertex.Normal.z, sizeof(float));
					Input.read((char*)&vertex.U, sizeof(float));
					Input.read((char*)&vertex.V, sizeof(float));
					//Calculate Size for AABB collision box
					if (vertex.Position.x > ReadMesh.CollisionBox.maxX)
					{
						ReadMesh.CollisionBox.maxX = vertex.Position.x;
					}
					if (vertex.Position.y > ReadMesh.CollisionBox.maxY)
					{
						ReadMesh.CollisionBox.maxY = vertex.Position.y;
					}
					if (vertex.Position.z > ReadMesh.CollisionBox.maxZ)
					{
						ReadMesh.CollisionBox.maxZ = vertex.Position.z;
					}
					if (vertex.Position.x < ReadMesh.CollisionBox.minX)
					{
						ReadMesh.CollisionBox.minX = vertex.Position.x;
					}
					if (vertex.Position.y < ReadMesh.CollisionBox.minY)
					{
						ReadMesh.CollisionBox.minY = vertex.Position.y;
					}
					if (vertex.Position.z < ReadMesh.CollisionBox.minZ)
					{
						ReadMesh.CollisionBox.minZ = vertex.Position.z;
					}
					ReadMesh.NumIndices++;
					Vertices.push_back(vertex);
				}

				//Read Indices


				for (int i = 0; i < NumIndices; i++)
				{
					int Index = 0;
					Input.read((char*)&Index, sizeof(int));
					Indices.push_back(Index);
					ReadMesh.NumIndices++;
				}

				//read materials
				std::string Name;
				ReadMesh.Vertices.push_back(Vertices);
				ReadMesh.Indices.push_back(Indices);
				size_t len;
				Input.read((char*)&len, sizeof(size_t));
				char* temp = new char[len + 1];
				Input.read(temp, len);
				temp[len] = '\0';
				Name = temp;
				delete[] temp;
				ReadMesh.Materials.push_back(Name);
				ReadMesh.NumIndices = NumIndices;
				ReadMesh.NumVertices = NumVertices;
			}
			Input.read((char*)&ReadMesh.CastShadow, sizeof(uint8_t));
			try
			{
				Input.read((char*)&ReadMesh.HasCollision, sizeof(uint8_t));
				Input.read((char*)&ReadMesh.TwoSided, sizeof(uint8_t));
			}
			catch (std::exception) {}
			Input.close();
			return ReadMesh;
		}
		throw "Cannot load model";
	}

	void JSM::SaveJSModel(ModelData Model, std::string Path)
	{
		std::ofstream Output(Path, std::ios::out | std::ios::binary);

		int NumMaterials = Model.Materials.size();
		Output.write((char*)&NumMaterials, sizeof(int));
		for (int j = 0; j < NumMaterials; j++)
		{
			int NumVertices = Model.Vertices[j].size();
			int NumIndices = Model.Indices[j].size();

			Output.write((char*)&NumVertices, sizeof(int));
			Output.write((char*)&NumIndices, sizeof(int));

			for (int i = 0; i < NumVertices; i++)
			{
				Output.write((char*)&Model.Vertices[j][i].Position.x, sizeof(float));
				Output.write((char*)&Model.Vertices[j][i].Position.y, sizeof(float));
				Output.write((char*)&Model.Vertices[j][i].Position.z, sizeof(float));
				Output.write((char*)&Model.Vertices[j][i].Normal.x, sizeof(float));
				Output.write((char*)&Model.Vertices[j][i].Normal.y, sizeof(float));
				Output.write((char*)&Model.Vertices[j][i].Normal.z, sizeof(float));
				Output.write((char*)&Model.Vertices[j][i].U, sizeof(float));
				Output.write((char*)&Model.Vertices[j][i].V, sizeof(float));
			}
			for (int i = 0; i < NumIndices; i++)
			{
				Output.write((char*)&Model.Indices[j][i], sizeof(int));
			}
			size_t size = ("Content/" + Model.Materials[j]).size();

			Output.write((char*)&size, sizeof(size_t));
			std::string OutPath = ("Content/" + Model.Materials[j]);

			Output.write(OutPath.c_str(), sizeof(char) * size);
		}
		Output.write((char*)&Model.CastShadow, sizeof(bool));
		Output.write((char*)&Model.HasCollision, sizeof(uint8_t));
		Output.write((char*)&Model.TwoSided, sizeof(uint8_t));
		Output.close();
	}
}