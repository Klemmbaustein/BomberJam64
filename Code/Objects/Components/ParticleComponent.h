#pragma once
#include <Objects/Components/Component.h>

class ParticleEmitter;

class ParticleComponent : public Component
{
	ParticleEmitter* Emitter;
public:
	void Start() override;
	void Tick() override;
	void Destroy() override;
	
	void LoadParticle(std::string Name);
	void SetActive(bool Active);
	bool GetActive();

	void SetPosition();
	Vector3 GetPosition();
};