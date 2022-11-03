#pragma once
#include "MaterialFunctions.h"
int MaterialUniformTypeStringToInt(std::string Type)
{
	if (Type == "Integer" || Type == "int" || Type == "num" || Type == "Int" || Type == "default" || Type == "bool" || Type == "i")
	{
		return 0;
	}
	if (Type == "Float" || Type == "float" || Type == "double" || Type == "f")
	{
		return 1;
	}
	if (Type == "Vector2" || Type == "vec2" || Type == "Vec2")
	{
		return 2;
	}
	if (Type == "Vector3" || Type == "vec3" || Type == "Vec3" || Type == "vec")
	{
		return 3;
	}
	if (Type == "Vector4" || Type == "vec4" || Type == "Vec4")
	{
		return 4;
	}

	if (Type == "Texture" || Type == "tex" || Type == "Tex" || Type == "texture")
	{
		return 5;
	}
	return -1;
}

std::string MaterialUniformTypeToString(int Type)
{
	switch (Type)
	{
	case 0:
		return "Integer";
	case 1:
		return "Float";
	case 2:
		return "Vector2";
	case 3:
		return "Vector3";
	case 4:
		return "Vector4";
	case 5:
		return "Texture";
	default:
		return "Unknown";
	}
}