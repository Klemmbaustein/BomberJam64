#include "Bomb.h"
#include <Objects/Objects.h>
#include <Sound/Sound.h>
#include <Log.h>
#include <EngineRandom.h>
#include <algorithm>
#include <Rendering/Camera/CameraShake.h>
#include <Objects/WallObject.h>
#include <Objects/PlayerObject.h>

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
			Sound::PlaySound3D(BombSound, GetTransform().Location, 5000.f, Random::GetRandomNumber(0.8f, 1.2f), 5*Random::GetRandomNumber(2.8f, 4.2f), false);
			CameraShake::PlayDefaultCameraShake(1.5f);
			auto AllWalls = Objects::GetAllObjectsWithID(6);

			for (auto* o : AllWalls)
			{
				if (Vector3::Distance(o->GetTransform().Location, GetTransform().Location) < 17)
				{
					Objects::DestroyObject(o);
				}
			}
			auto Player = dynamic_cast<PlayerObject*>(Objects::GetAllObjectsWithID(4)[0]);

			if (Vector3::Distance(Player->GetTransform().Location, GetTransform().Location) < 16)
			{
				Player->Health -= 60.0f;
				CameraShake::PlayDefaultCameraShake(1.5f);
			}
			PlayedSound = true;
		}
		ExplosionMesh->SetVisibility(true);
		BombMesh->SetVisibility(false);
		ExplosionMesh->SetUniform("u_opacity", U_FLOAT, std::to_string(std::max(1.0f - (-DetonationTime * 3.0f), 0.0f)), 0);
		ExplosionMesh->SetUniform("u_opacity", U_FLOAT, std::to_string(std::max(1.0f - (-DetonationTime * 3.0f), 0.0f)), 1);

		ExplosionMesh->SetRelativeTransform(Transform(Vector3(), Vector3(), Vector3(15.f * -DetonationTime)));

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
