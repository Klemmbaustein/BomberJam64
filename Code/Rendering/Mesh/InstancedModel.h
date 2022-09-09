#pragma once
#include <Rendering/Mesh/Model.h>
#include <Rendering/Mesh/InstancedMesh.h>
class InstancedModel : public Renderable
{
public:
	InstancedModel(std::string Filename);
	~InstancedModel();
	void Render(Camera* WorldCamera) override;
	virtual void SimpleRender(Shader* Shader) override;
	std::vector<glm::mat4> MatModel = { glm::mat4(1.f) };
	Collision::Box Size;
	Vector3 ModelCenter;
	Transform ModelTransform;
	bool TwoSided = false;
	size_t AddInstance(Transform T);
	bool RemoveInstance(size_t Inst);
	void ConfigureVAO();
	void LoadMaterials(std::vector<std::string> Materials);
protected:
	std::vector<Transform> Instances;
	unsigned int MatBuffer = -1;
	std::vector<InstancedMesh*> Meshes;
	glm::mat4 ModelViewProjection = glm::mat4();
	std::vector<std::string> Materials;
	Collision::Box NonScaledSize;
	std::vector<MaterialUniforms::Param> Uniforms;
};