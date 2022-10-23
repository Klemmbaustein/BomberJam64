#pragma once
#include "Mesh.h"
#include <vector>
#include <string>
#include "Rendering/Camera/Camera.h"
#include "glm/glm.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "Math/Collision/CollisionBox.h"
#include <UI/EngineUI/MaterialFunctions.h>
#include <Rendering/Renderable.h>
#include <Rendering/Camera/FrustumCulling.h>
namespace MaterialUniforms
{
	struct Param
	{
		std::string UniformName;
		int Type;
		std::string Value;
		Param(std::string UniformName, int Type, std::string Value)
		{
			this->UniformName = UniformName;
			this->Type = Type;
			this->Value = Value;
		}
	};
}
struct MeshData
{
	std::vector<Vertex> Vertices;
	std::vector<int> Indices;
};
class Model : public Renderable
{
public:
	Model(std::string Filename);

	Model()
	{
		CastShadow = true;
	}

	~Model();

	//Render

	virtual void Render(Camera* WorldCamera) override;

	void LoadMaterials(std::vector<std::string> Materials);

	virtual void UpdateTransform()
	{
		MatModel = glm::mat4(1.f);

		Vector3 Scale = NonScaledSize * ModelTransform.Scale;
		Size = FrustumCulling::AABB(ModelTransform.Location, Scale.X, Scale.Y, Scale.Z);
		MatModel = glm::translate(MatModel, (glm::vec3)ModelTransform.Location);
		MatModel = glm::rotate(MatModel, ModelTransform.Rotation.Y, glm::vec3(0, 1, 0));
		MatModel = glm::rotate(MatModel, ModelTransform.Rotation.Z, glm::vec3(0, 0, 1));
		MatModel = glm::rotate(MatModel, ModelTransform.Rotation.X, glm::vec3(1, 0, 0));

		MatModel = glm::scale(MatModel, (glm::vec3)ModelTransform.Scale * 0.025f);
		ConfigureVAO();
	}

	//Basic Translation

	void MultiplyScale(Vector3 Multiplier)
	{
		ModelTransform.Scale.X *= Multiplier.X;
		ModelTransform.Scale.Y *= Multiplier.Y;
		ModelTransform.Scale.Z *= Multiplier.Z;
	}

	void Rotate(float Angle, Vector3 Axis)
	{
		ModelTransform.Rotation.X += Angle * Axis.X;
		ModelTransform.Rotation.Y += Angle * Axis.Y;
		ModelTransform.Rotation.Z += Angle * Axis.Z;
	}

	void AddOffset(Vector3 Offset)
	{
		ModelTransform.Location.X += Offset.X;
		ModelTransform.Location.Y += Offset.Y;
		ModelTransform.Location.Z += Offset.Z;
	}
	void ConfigureVAO();

	virtual void SimpleRender(Shader* Shader) override;
	glm::mat4 MatModel = glm::mat4(1.f);
	Vector3 ModelCenter;
	Transform ModelTransform;
	bool TwoSided = false, HasCollision = true;
	FrustumCulling::AABB Size = FrustumCulling::AABB(Vector3(), NonScaledSize, NonScaledSize, NonScaledSize);
	void SetUniform(Uniform NewUniform, uint8_t MeshIndex);

	bool Visible = true;

	unsigned int MatBuffer = -1;
	std::vector<Mesh*> Meshes;
	MeshData ModelMeshData;
protected:
	glm::mat4 ModelViewProjection = glm::mat4();
	std::vector<std::string> Materials;
	float NonScaledSize = 1;
};