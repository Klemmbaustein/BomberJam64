#pragma once
#include "Math/Vector.h"
#include "Rendering/Shader.h"
#include "Rendering/VertexBuffer.h"
#include <vector>
#include "Rendering/Utility/IndexBuffer.h"
#include <Rendering/Mesh/Mesh.h>
class InstancedMesh
{
public:
	InstancedMesh(std::vector<Vertex> Vertices, std::vector<int> Indices);

	void Render(Shader* Shader);

	void SimpleRender(Shader* Shader);

	~InstancedMesh();

	void ApplyUniforms();

	bool IsTransparent = false;
	std::vector<Uniform> Uniforms;
	std::string VertexShader, FragmentShader;
	Shader* MeshShader;
	void SetInstances(std::vector<Transform> T);
	VertexBuffer* MeshVertexBuffer;

protected:
private:
	std::vector<Transform> Instances;
	IndexBuffer* MeshIndexBuffer;
	int NumIndices;
	int NumVertices;
};

