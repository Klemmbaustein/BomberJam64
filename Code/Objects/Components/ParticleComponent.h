#pragma once
#include <Objects/Components/Component.h>

namespace Particles
{
	class ParticleEmitter;
}

class ParticleComponent : public Component
{
	Particles::ParticleEmitter* Emitter;
	Vector3 Position;
public:
	void Start() override;
	void Tick() override;
	void Destroy() override;
	
	void LoadParticle(std::string Name);
	void SetActive(bool Active);
	bool GetActive();

	void SetRelativePosition(Vector3 NewPos);
	Vector3 GetRelativePosition();
};