#include "ShaderManager.h"
#include <Log.h>

std::map<ShaderDescription, ShaderElement> Shaders;


bool operator<(ShaderDescription a, ShaderDescription b)
{
	return a.VertexShader < b.VertexShader || a.FragmentShader < b.FragmentShader;
}

Shader* ReferenceShader(std::string VertexShader, std::string FragmentShader)
{
	ShaderDescription ShaderToFind;
	ShaderToFind.FragmentShader = FragmentShader;
	ShaderToFind.VertexShader = VertexShader;
	auto FoundShader = Shaders.find(ShaderToFind);
	if (!Shaders.contains(ShaderToFind))
	{
		Shader* NewShader = new Shader(VertexShader.c_str(), FragmentShader.c_str());
		Shaders.insert(std::make_pair(ShaderToFind, ShaderElement(NewShader, 1)));
		return NewShader;
	}
	else
	{
		FoundShader->second.References++;
		return FoundShader->second.Shader;
	}
}

size_t GetNumShaders()
{
	return Shaders.size();
}

Shader* GetShader(std::string VertexShader, std::string FragmentShader)
{
	ShaderDescription ShaderToFind;
	ShaderToFind.FragmentShader = FragmentShader;
	ShaderToFind.VertexShader = VertexShader;
	auto FoundShader = Shaders.find(ShaderToFind);
	if (FoundShader == Shaders.end())
	{
		return nullptr;
	}
	return FoundShader->second.Shader;
}

void DereferenceShader(std::string VertexShader, std::string FragmentShader)
{
	ShaderDescription ShaderToFind;
	ShaderToFind.FragmentShader = FragmentShader;
	ShaderToFind.VertexShader = VertexShader;
	auto FoundShader = Shaders.find(ShaderToFind);
	if (!Shaders.count(ShaderToFind))
	{
		Log::CreateNewLogMessage("Error: Could not dereference Shader" + VertexShader + FragmentShader, Vector3(1, 0, 0));
	}
	else
	{
		FoundShader->second.References--;
		if (FoundShader->second.References <= 0)
		{
			delete FoundShader->second.Shader;
			Shaders.erase(FoundShader);
		}
	}
}

void DereferenceShader(Shader* Shader)
{

}
