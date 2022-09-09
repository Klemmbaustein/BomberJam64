#include "InstancedMesh.h"
#include "Math/Vector.h"
#include "Rendering/Material.h"
#include "Rendering/Shader.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/Utility/IndexBuffer.h"
#include <vector>
#include <fstream>
#include <Rendering/Texture/Texture.h>
#include <iostream>
#include <WorldParameters.h>

InstancedMesh::InstancedMesh(std::vector<Vertex> Vertices, std::vector<int> Indices)
{
	NumVertices = Vertices.size();
	NumIndices = Indices.size();
	MeshIndexBuffer = new IndexBuffer(Indices.data(), NumIndices, sizeof(Indices[0]));
	MeshVertexBuffer = new VertexBuffer(Vertices.data(), NumVertices);

}


InstancedMesh::~InstancedMesh()
{
	delete MeshVertexBuffer;
	delete MeshIndexBuffer;
	for (Uniform& u : Uniforms)
	{
		delete u.Content;
	}
}

void InstancedMesh::SimpleRender(Shader* Shader)
{
	if (IsTransparent)
	{
		for (int i = 0; i < Uniforms.size(); ++i)
		{
			switch (Uniforms.at(i).Value)
			{
			case 0:
				glUniform1iv(glGetUniformLocation(Shader->GetShaderID(), Uniforms.at(i).Name.c_str()), 1, static_cast<int*>(Uniforms.at(i).Content));
				break;
			case 1:
				glUniform1fv(glGetUniformLocation(Shader->GetShaderID(), Uniforms.at(i).Name.c_str()), 1, static_cast<float*>(Uniforms.at(i).Content));
				break;
			case 3:
				glUniform3f(glGetUniformLocation(Shader->GetShaderID(), Uniforms.at(i).Name.c_str()), static_cast<Vector3*>((Uniforms.at(i).Content))->X, static_cast<Vector3*>((Uniforms.at(i).Content))->Y, static_cast<Vector3*>((Uniforms.at(i).Content))->Z);
				break;
			case 5:
				glActiveTexture(GL_TEXTURE7);
				glBindTexture(GL_TEXTURE_2D, *(unsigned int*)Uniforms.at(i).Content);
				glUniform1i(glGetUniformLocation(Shader->GetShaderID(), Uniforms.at(i).Name.c_str()), 7);
				break;
			default:
				glUniform1iv(glGetUniformLocation(Shader->GetShaderID(), Uniforms.at(i).Name.c_str()), 1, static_cast<int*>(Uniforms.at(i).Content));
				break;
			}
		}
	}
	else glUniform1i(glGetUniformLocation(Shader->GetShaderID(), "u_usetexture"), 0);
	MeshVertexBuffer->Bind();
	MeshIndexBuffer->Bind();
	glDrawElementsInstanced(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, 0, Instances.size());
	MeshVertexBuffer->Unbind();
	MeshIndexBuffer->Unbind();
}

void InstancedMesh::ApplyUniforms()
{
	for (Uniform& u : Uniforms)
	{
		switch (u.Value)
		{
		case 0:
			u.Content = new int(std::stoi(static_cast<char*>(u.Content)));
			break;
		case 1:
			u.Content = new float(std::stof(static_cast<char*>(u.Content)));
			break;
		case 3:
			u.Content = new Vector3(Vector3::stov(static_cast<char*>(u.Content)));
			break;
		case 5:
			u.Content = (void*)new unsigned int(Texture::LoadTexture(std::string(static_cast<char*>(u.Content)) + ".png"));
			break;
		default:
			break;
		}
	}
}

void InstancedMesh::SetInstances(std::vector<Transform> T)
{
	Instances = T;
}