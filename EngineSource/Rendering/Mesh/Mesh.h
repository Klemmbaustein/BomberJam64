#pragma once
#include "Math/Vector.h"
#include "Rendering/Shader.h"
#include "Rendering/VertexBuffer.h"
#include <vector>
#include "Rendering/Utility/IndexBuffer.h"

struct Uniform
{ 
	std::string Name;
	int Value;
	void* Content;
	Uniform(std::string Name, int Value, void* Content)
	{
		this->Content = Content;
		this->Name = Name;
		this->Value = Value;
	}
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> Vertices, std::vector<int> Indices);

	void Render(Shader* Shader);


	void SimpleRender(Shader* Shader);

	~Mesh();

	void ApplyUniforms();
	void ApplyUniform(size_t Index);
	bool IsTransparent = false;
	std::vector<Uniform> Uniforms;
	std::string VertexShader, FragmentShader;
	Shader* MeshShader = nullptr;
	VertexBuffer* MeshVertexBuffer = nullptr;

protected:
private:
	IndexBuffer* MeshIndexBuffer;
	int NumIndices;
	int NumVertices;
};

