#pragma once
#include "Math/Vector.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/Utility/IndexBuffer.h"
#include "Rendering/Vertex.h"
#include "glm/glm.hpp"
#include <GL/glew.h>
#include "Rendering/Shader.h"

class ScrollObject;

class UIBorder
{
public:
	UIBorder(Vector2 Position, Vector2 Scale, Vector3 Color, GLuint TextureID = (unsigned int)0, bool UseTexture = false, bool FlipTexture = false)
	{
		this->Color = Color;
		this->Position = Position;
		this->Scale = Scale;
		
		Vertex vertex1;
		vertex1.Position.x = Position.X;
		vertex1.Position.y = Position.Y;
		vertex1.Position.z = 0;
		vertex1.U = 1;
		vertex1.V = 1;
		Vertices.push_back(vertex1);

		Vertex vertex2;
		vertex2.Position.x = Position.X + Scale.X;
		vertex2.Position.y = Position.Y;
		vertex2.Position.z = 0;
		vertex2.U = 0;
		vertex2.V = 1;
		Vertices.push_back(vertex2);

		Vertex vertex3;
		vertex3.Position.x = Position.X;
		vertex3.Position.y = Position.Y + Scale.Y;
		vertex3.Position.z = 0;
		vertex3.U = 1;
		vertex3.V = 0;
		Vertices.push_back(vertex3);

		Vertex vertex4;
		vertex4.Position.x = Position.X + Scale.X;
		vertex4.Position.y = Position.Y + Scale.Y;
		vertex4.Position.z = 0;
		vertex4.U = 0;
		vertex4.V = 0;
		Vertices.push_back(vertex4);
		if (FlipTexture)
		{
			for (int i = 0; i < Vertices.size(); i++)
			{
				Vertices.at(i).U = 1 - Vertices.at(i).U;
				Vertices.at(i).V = 1 - Vertices.at(i).V;
			}
		}
		MeshVertexBuffer = new VertexBuffer(Vertices.data(), Vertices.size());
		this->UseTexture = UseTexture;
		this->TextureID = TextureID;
		this->FlipTexture = FlipTexture;
	}


	void ReInit(Vector2 Position, Vector2 Scale, Vector3 Color, GLuint TextureID = (unsigned int)0, Vector2 TextureSize = Vector2(0), bool UseTexture = false, bool FlipTexture = false)
	{
		delete MeshVertexBuffer;
		MeshVertexBuffer = nullptr;
		this->Color = Color;
		this->Position = Position;
		this->Scale = Scale;

		Vertices.clear();

		Vertex vertex1;
		vertex1.Position.x = Position.X;
		vertex1.Position.y = Position.Y;
		vertex1.Position.z = 0;
		vertex1.U = 1;
		vertex1.V = 1;
		Vertices.push_back(vertex1);

		Vertex vertex2;
		vertex2.Position.x = Position.X + Scale.X;
		vertex2.Position.y = Position.Y;
		vertex2.Position.z = 0;
		vertex2.U = 0;
		vertex2.V = 1;
		Vertices.push_back(vertex2);

		Vertex vertex3;
		vertex3.Position.x = Position.X;
		vertex3.Position.y = Position.Y + Scale.Y;
		vertex3.Position.z = 0;
		vertex3.U = 1;
		vertex3.V = 0;
		Vertices.push_back(vertex3);

		Vertex vertex4;
		vertex4.Position.x = Position.X + Scale.X;
		vertex4.Position.y = Position.Y + Scale.Y;
		vertex4.Position.z = 0;
		vertex4.U = 0;
		vertex4.V = 0;
		Vertices.push_back(vertex4);
		if (FlipTexture)
		{
			for (int i = 0; i < Vertices.size(); i++)
			{
				Vertices.at(i).U = 1 - Vertices.at(i).U;
				Vertices.at(i).V = 1 - Vertices.at(i).V;
			}
		}
		MeshVertexBuffer = new VertexBuffer(Vertices.data(), Vertices.size());
		this->UseTexture = UseTexture;
		this->TextureID = TextureID;
		this->FlipTexture = FlipTexture;
	}

	void SetTexture(GLuint TextureID = (unsigned int)0, bool UseTexture = false)
	{
		this->UseTexture = UseTexture;
		this->TextureID = TextureID;
	}

	virtual void Render(Shader* Shader);

	virtual ~UIBorder();
	void ScrollTick(Shader* UsedShader);
	GLuint TextureID;
	bool UseTexture;
	bool FlipTexture;
	ScrollObject* CurrentScrollObject = nullptr;
	float Opacity = 1.0f;
	Vector2 Position = Vector2();
	Vector2 Scale = Vector2();
	Vector3 Color = Vector3();
protected:
	VertexBuffer* MeshVertexBuffer = nullptr;
	std::vector<Vertex> Vertices;
private:
	friend ScrollObject;
};