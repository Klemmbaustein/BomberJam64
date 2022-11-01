#pragma once

#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <vector>
#include <assimp/postprocess.h>
#include "Math/Vector.h"
#include <fstream>
#include "Engine/FileUtility.h"

/*
* MODEL IMPORTER USING ASSIMP
* 
* MOSTLY TESTED WITH .FBX
*/


struct ImportMesh
{
	std::vector<Vector3> Positions;
	std::vector<Vector3> Normals;
	std::vector<Vector2> UVs;
	std::vector<int> Indicies;
};

extern std::vector<ImportMesh> Meshes;

namespace ModelImporter
{
	std::string Import(std::string Name, std::string CurrentFilepath);
}