#pragma once

#include <iostream>
#include <assimp/importer.hpp>
#include <assimp/scene.h>
#include <vector>
#include <assimp/postprocess.h>
#include "Math/vector.h"
#include <fstream>
#include "Rendering/Material.h"
#include "FileUtility.h"

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
	Material Material;
};

extern std::vector<ImportMesh> Meshes;
extern std::vector<Material> Materials;

namespace ModelImporter
{
	std::string Import(std::string Name, std::string CurrentFilepath);
}