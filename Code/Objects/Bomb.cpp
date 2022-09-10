#include "Bomb.h"
#include <Objects/Objects.h>
#include <Sound/Sound.h>
#include <Log.h>
#include <EngineRandom.h>
#include <algorithm>
#include <Rendering/Camera/CameraShake.h>

Sound::SoundBuffer* BombSound = nullptr;

void Bomb::Begin()
{
	if (!BombSound)
	{
		BombSound = Sound::LoadSound("Explosion");
	}

	BombMesh = new MeshComponent();
	Attach(BombMesh);
	BombMesh->Load("Bomb");

	ExplosionMesh = new MeshComponent();
	Attach(ExplosionMesh);
	ExplosionMesh->Load("Explosion");
	ExplosionMesh->SetVisibility(false);
	BombMesh->SetRelativeTransform(Transform(Vector3(0, -2, 0), Vector3(), Vector3(0.7f)));
}

void Bomb::Tick()
{
	DetonationTime -= Performance::DeltaTime;
	if (DetonationTime < 0)
	{
		if (!PlayedSound)
		{
			Sound::PlaySound2D(BombSound, Random::GetRandomNumber(0.8f, 1.2f), Random::GetRandomNumber(0.8f, 1.2f), false);
			CameraShake::PlayDefaultCameraShake(1.2f);
			PlayedSound = true;
		}
		ExplosionMesh->SetVisibility(true);
		BombMesh->SetVisibility(false);
		ExplosionMesh->SetUniform("u_opacity", U_FLOAT, std::to_string(std::max(1.0f - (-DetonationTime * 5.0f), 0.0f)), 0);
		ExplosionMesh->SetUniform("u_opacity", U_FLOAT, std::to_string(std::max(1.0f - (-DetonationTime * 5.0f), 0.0f)), 1);

		ExplosionMesh->SetRelativeTransform(Transform(Vector3(), Vector3(), Vector3(10.f * -DetonationTime)));

		if (DetonationTime < -0.4f)
		{
			Objects::DestroyObject(this);
			return;
		}
	}
}

void Bomb::Destroy()
{
}
