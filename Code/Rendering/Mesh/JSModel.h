#pragma once
#include <string>
#include <vector>
#include <Rendering/Vertex.h>
#include <Math/Collision/CollisionBox.h>
namespace JSM
{
	struct ModelData
	{
		int NumVertices = 0, NumIndices = 0;
		std::vector<std::vector<Vertex>> Vertices;
		std::vector<std::vector<int>> Indices;
		std::vector<std::string> Materials;
		Collision::Box CollisionBox;
		bool CastShadow = false, TwoSided = false, HasCollision = false;
	};
	ModelData LoadJSModel(std::string Path);
	void SaveJSModel(ModelData Model, std::string Path);
}