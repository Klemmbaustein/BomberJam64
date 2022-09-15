#pragma once
#include "WorldObject.h"
#include "Components/CameraComponent.h"
#include "Components/CollisionComponent.h"
#include <Sound/Sound.h>
#include <UI/Game/GameUI.h>

constexpr unsigned int PLAYER_NUM_ANIM_FRAMES = 10;


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
protected:
	void TryLoadSave();

	Vector3 SpawnPoint;
	GameUI* UI = nullptr;
	CollisionComponent* PlayerCollider = nullptr;
	CollisionComponent* PlayerCollider2 = nullptr;
	CameraComponent* PlayerCamera = nullptr;
	Sound::SoundBuffer* Footstepsounds = Sound::LoadSound("Walk");
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
	float TeleportCancelTime = 3.f;
	bool LoadedSave = false;
	std::string Levels[3]
	{
		"ForestLevel",
		"TestScene",
		"ForestLevel"
	};


	std::string AllAnimMeshes[PLAYER_NUM_ANIM_FRAMES] =
	{
		"Idle",		//Anim 00
		"Walk_0",	//Anim 01
		"Walk_1",	//Anim 02
		"Walk_2",	//Anim 03
		"Walk_3",	//Anim 04
		"Idle_B",	//Anim 05
		"Walk_0-B",	//Anim 06
		"Walk_1-B",	//Anim 07
		"Walk_2-B",	//Anim 08
		"Walk_3-B"	//Anim 09
	};

	struct Animation { unsigned int Frames; unsigned int StartingFrame; };
	Animation Anims[4] = 
	{
		{0, 0},
		{3, 1},
		{0, 5},
		{3, 6}
	};

	MeshComponent* AllAnimComponents[PLAYER_NUM_ANIM_FRAMES] = {};

	unsigned int CurrentAnim = 0;
	unsigned int CurrentAnimFrame = 0;
	float AnimFrameTimer = 0.0f;

};