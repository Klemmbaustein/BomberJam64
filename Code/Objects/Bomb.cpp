#include "Bomb.h"
#include <Objects/Objects.h>
#include <Sound/Sound.h>
#include <Log.h>

Sound::SoundBuffer* BombSound = Sound::LoadSound("Explosion");

void Bomb::Begin()
{
	BombMesh = new MeshComponent();
	Attach(BombMesh);
	BombMesh->Load("Bomb");

	BombMesh->SetRelativeTransform(Transform(Vector3(), Vector3(), Vector3(0.4f)));
}

void Bomb::Tick()
{
	DetonationTime -= Performance::DeltaTime;
	if (DetonationTime < 0)
	{
		Objects::DestroyObject(this);
		Sound::PlaySound3D(BombSound, GetTransform().Location, 1000, 1, 1, false);
		return;
	}
}

void Bomb::Destroy()
{
}
