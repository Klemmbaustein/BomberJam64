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

	void Render(Shader* Shader)
	{
		MeshVertexBuffer->Bind();
		MeshIndexBuffer->Bind();
		uint8_t TexIterator = 0;
		for (int i = 0; i < Uniforms.size(); ++i)
		{

			switch (Uniforms.at(i).Value)
			{
			case 0:
				glUniform1iv(glGetUniformLocation(Shader->GetShaderID(), Uniforms.at(i).Name.c_str()), 1, static_cast<int*>(Uniforms.at(i).Content));
				break;
			case 1:
				glUniform1fv(glGetUniformLocation(Shader->GetShaderID(), Uniforms.at(i).Name.c_str()), 1, (float*)Uniforms[i].Content);
				break;
			case 3:
				glUniform3f(glGetUniformLocation(Shader->GetShaderID(), Uniforms.at(i).Name.c_str()), static_cast<Vector3*>((Uniforms.at(i).Content))->X, static_cast<Vector3*>((Uniforms.at(i).Content))->Y, static_cast<Vector3*>((Uniforms.at(i).Content))->Z);
				break;
			case 5:
				glActiveTexture(GL_TEXTURE7 + TexIterator);
				glBindTexture(GL_TEXTURE_2D, *(unsigned int*)Uniforms.at(i).Content);
				glUniform1i(glGetUniformLocation(Shader->GetShaderID(), Uniforms.at(i).Name.c_str()), 7 + TexIterator);
				TexIterator++;
				break;
			default:
				glUniform1iv(glGetUniformLocation(Shader->GetShaderID(), Uniforms.at(i).Name.c_str()), 1, static_cast<int*>(Uniforms.at(i).Content));
				break;
			}
		}
		unsigned int attachements[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
		glDrawBuffers(5, attachements);
		glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, 0);
		MeshVertexBuffer->Unbind();
		MeshIndexBuffer->Unbind();
	}

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

