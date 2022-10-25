#pragma once
#include "WorldObject.h"
#include "Components/CameraComponent.h"
#include "Components/CollisionComponent.h"
#include <Sound/Sound.h>
#include <UI/Game/GameUI.h>
#include <GENERATED/GENERATED_PlayerObject.h>

constexpr unsigned int PLAYER_NUM_ANIM_FRAMES = 10;
class PlayerObject : public WorldObject
{
public:
	PLAYEROBJECT_GENERATED()
		
	Vector3 Rotation = glm::vec3(0);
	void Tick() override;
	void Begin() override;
	void Destroy() override;
	bool TryMove(Vector3 Offset, bool Vertical);
	void Respawn();

	const float MovementSpeed = 5;
	bool HasCollision = true;
	Vector2 Velocity = Vector2(0);
	bool OnGround = false;
	float BombTime = 0.0f;
	uint32_t NumOrbs = 0;
	float Health = 100.0f;
	GameUI* UI = nullptr;

protected:
	void TryLoadSave();

	Vector3 SpawnPoint;
	CollisionComponent* PlayerCollider = nullptr;
	CollisionComponent* PlayerCollider2 = nullptr;
	CameraComponent* PlayerCamera = nullptr;
	Sound::SoundBuffer* Footstepsounds = Sound::LoadSound("Walk");
	Vector3 CameraRotation;
	float BombLayTime = 0;
	float VerticalVelocity = 0;
	bool IsDead = false;
	Vector3 PrevCameraPos = 0;
	bool HasJumped = false;
	bool IsClicking = false;
	bool InLevelTransition = false;
	float TeleportCancelTime = 3.f;
	bool LoadedSave = false;
	float MaxBombs = 5.0f;
	float Time = 0.f;

	bool End = false;
	MeshComponent* AllAnimComponents[PLAYER_NUM_ANIM_FRAMES] = {};

	unsigned int CurrentAnim = 0;
	unsigned int CurrentAnimFrame = 0;
	float AnimFrameTimer = 0.0f;
};