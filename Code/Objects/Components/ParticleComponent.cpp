#include "ParticleComponent.h"
#include <filesystem>
#include <Rendering/Particle.h>
#include <World/Assets.h>
#include <Rendering/Mesh/Model.h>
#include <World/Graphics.h>
#include <Rendering/Utility/Framebuffer.h>
#include <Log.h>

void ParticleComponent::Start()
{
	Emitter = new Particles::ParticleEmitter();
	Graphics::MainFramebuffer->ParticleEmitters.push_back(Emitter);
}

void ParticleComponent::Tick()
{
	Emitter->Position = Position + GetParent()->GetTransform().Location;
}
void ParticleComponent::Destroy()
{
	delete Emitter;
}
void ParticleComponent::LoadParticle(std::string Name)
{
	std::vector<std::string> ElementMaterials;
	std::string File = Assets::GetAsset(Name + ".jspart");
	for (unsigned int i = 0; i < Emitter->ParticleVertexBuffers.size(); i++)
	{
		delete Emitter->ParticleVertexBuffers[i];
		delete Emitter->ParticleIndexBuffers[i];
	}
	Emitter->ParticleVertexBuffers.clear();
	Emitter->ParticleIndexBuffers.clear();
	Emitter->SpawnDelays.clear();
	Emitter->ParticleShaders.clear();
	Emitter->ParticleInstances.clear();
	Emitter->Uniforms.clear();
	Emitter->ParticleMatrices.clear();
	Emitter->ParticleElements.clear();

	if (std::filesystem::exists(File))
	{
		if (!std::filesystem::is_empty(File))
		{
			auto ParticleData = Particles::ParticleEmitter::LoadParticleFile(File, ElementMaterials);
			for (unsigned int i = 0; i < ParticleData.size(); i++)
			{
				Emitter->AddElement(ParticleData[i]);
				Emitter->SetMaterial(i, Assets::GetAsset(ElementMaterials[i] + ".jsmat"));
			}
		}
	}
	else
	{
		Log::CreateNewLogMessage("Particle emitter " + Name + " does not exist");
	}
}
void ParticleComponent::SetActive(bool Active)
{
	Emitter->Active = Active;
}
bool ParticleComponent::GetActive()
{
	return Emitter->Active;
}
void ParticleComponent::SetRelativePosition(Vector3 NewPos)
{
	Position = NewPos;
}
Vector3 ParticleComponent::GetRelativePosition()
{
	return Position;
}