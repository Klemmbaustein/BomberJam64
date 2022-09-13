#pragma once
#include "WorldObject.h"
#include "Components/CameraComponent.h"
#include "Components/CollisionComponent.h"
#include <Sound/Sound.h>
#include <UI/Game/GameUI.h>

class PlayerObject : public WorldObject
{
public:
	Vector3 Rotation = glm::vec3(0);
	PlayerObject() : WorldObject(ObjectDescription("PlayerObject", 4)) {}
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
	~PlayerObject();
protected:
	void TryLoadSave();

	Vector3 SpawnPoint;
	GameUI* UI = nullptr;
	CollisionComponent* PlayerCollider = nullptr;
	CollisionComponent* PlayerCollider2 = nullptr;
	CameraComponent* PlayerCamera = nullptr;
	MeshComponent* PlayerMesh = nullptr;
	Vector3 CameraRotation;
	float BombLayTime = 0;
	float VerticalVelocity = 0;
	float Time = 0;
	bool IsDead = false;
	float CameraDistance = 35;
	float Gravity = 200;
	float Speed = 750;
	Vector3 PrevCameraPos = 0;
	bool HasJumped = false;
	bool IsClicking = false;
	bool SpaceDown = false;
	bool InLevelTransition = false;
	float TeleportCancelTime = 1.f;
	bool LoadedSave = false;
	std::string Levels[3]
	{
		"ForestLevel",
		"TestScene",
		"ForestLevel"
	};
};