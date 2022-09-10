#include "PlayerObject.h"
#include <Input.h>
#include <Math/Collision/Collision.h>
#include <WorldParameters.h>
#include <Rendering/Mesh/JSModel.h>
#include <Rendering/Camera/CameraShake.h>
#include <EngineRandom.h>
#include <UI/UI.h>
#include <Log.h>
#include <World.h>
#include <Timer.h>
#include <Save.h>
#include <Objects/Bomb.h>
#include <Objects/Objects.h>

std::string NextLevel;
void LoadNextLevel()
{
	World::ScheduleLoadNewScene(NextLevel);
}


void PlayerObject::Tick()
{
	if (!IsInEditor)
	{
		if (IsDead)
		{
			return;
		}
		Vector2 InputVelocity = Vector2(0);
		if (Input::IsKeyDown(SDLK_w))
		{
			InputVelocity += Vector2(1.f, 0.f);
		}
		if (Input::IsKeyDown(SDLK_s))
		{
			InputVelocity += Vector2(-1.f, 0.f);
		}
		if (Input::IsKeyDown(SDLK_a))
		{
			InputVelocity += Vector2(0.f, -1.f);
		}
		if (Input::IsKeyDown(SDLK_d))
		{
			InputVelocity += Vector2(0.f, 1.f);
		}


		if (Input::IsKeyDown(SDLK_SPACE) && BombLayTime < 0)
		{
			BombLayTime = 0.2f;
			Objects::SpawnObject<Bomb>(GetTransform() + Transform(Vector3(), Vector3(0, Random::GetRandomNumber(-100, 100), 0), Vector3(1)));
		}

		BombLayTime -= Performance::DeltaTime;

		//OnGround = true;

		if (InputVelocity != Vector2(0))
		{
			InputVelocity = InputVelocity / Vector2(sqrt(InputVelocity.X * InputVelocity.X + InputVelocity.Y * InputVelocity.Y));
		}
		if (!OnGround)
		{
			InputVelocity = InputVelocity / Vector2(10);
		}
		Vector3 ForwardVel = Vector3::GetForwardVector(glm::vec3(0, Rotation.Y, 0)) * InputVelocity.X * Performance::DeltaTime * Speed;


		ForwardVel += Vector3::Cross(Vector3::GetForwardVector(Vector3(0, Rotation.Y, 0)), Vector3(0, 1, 0)) * InputVelocity.Y * Performance::DeltaTime * Speed;
		Velocity += Vector2(ForwardVel.Z, ForwardVel.X);
		if (ForwardVel != glm::vec3(0))
		{
			ForwardVel = ForwardVel.Normalize();
		}
		if (OnGround)
		{
			Velocity.X = std::lerp(Velocity.X, 0, std::min(Performance::DeltaTime * 15.f, 1.f));
			Velocity.Y = std::lerp(Velocity.Y, 0, std::min(Performance::DeltaTime * 15.f, 1.f));
		}
		else
		{
			Velocity.X = std::lerp(Velocity.X, 0, std::min(Performance::DeltaTime * 2.25f, 1.f));
			Velocity.Y = std::lerp(Velocity.Y, 0, std::min(Performance::DeltaTime * 2.25f, 1.f));
		}
		if (abs(Velocity.X) < 0.05f)
		{
			Velocity.X = 0;
		}
		if (abs(Velocity.Y) < 0.05f)
		{
			Velocity.Y = 0;
		}
		auto BoxHit = PlayerCollider2->CollMesh.OverlapCheck({ PlayerCollider });

		OnGround = BoxHit.Hit;

		if (!OnGround || VerticalVelocity > 0)
		{
			VerticalVelocity -= Performance::DeltaTime * Gravity;
			HasJumped = false;
		}
		else
		{
			VerticalVelocity = std::max(VerticalVelocity, 0.f);
		}

		TryMove(Vector3(Velocity.Y, 0, Velocity.X) * Vector3(Performance::DeltaTime), false);
		if (!TryMove(Vector3(0, VerticalVelocity * Performance::DeltaTime, 0), true))
		{
			if (VerticalVelocity < 0)
			{
				OnGround = true;
				VerticalVelocity = 0;
			}
			VerticalVelocity = std::min(VerticalVelocity, 0.f);
		}

		if (GetTransform().Location.Y < -1000.f && !IsDead)
		{
			CameraShake::PlayDefaultCameraShake(3);
			//UI->PlayTransition();
			IsDead = true;
		}

		CameraRotation += Vector3(Vector2(Input::MouseMovement.Y, Input::MouseMovement.X), 0) * 3;
		Rotation = Rotation * 0.8 + CameraRotation * 0.2;
		//Clamp the camera rotation so the camera stays somewhat top-down-ish;
		CameraRotation = Vector3(CameraRotation.X = std::max(std::min(CameraRotation.X, -35.f), -85.f), CameraRotation.Y, CameraRotation.Z);

		Vector3 CameraOffset = (PrevCameraPos) * 0.9 + (Vector3::GetForwardVector(Rotation) * -CameraDistance) * 0.1 + Vector3(0, 1, 0);
		if (Vector3(Velocity.Y, 0, Velocity.X).Length() > 0.25)
		{
			PlayerMesh->SetRelativeTransform(Transform(Vector3(),
				Vector3::LookAtFunction(Vector3(), Vector3(Velocity.Y, 0, Velocity.X), true),
				Vector3(0.5, 2, 0.5)));
		}
		PrevCameraPos = CameraOffset;
		auto GroundHit = Collision::LineTrace(
			GetTransform().Location + Vector3(0, 0.5, 0),
			GetTransform().Location + CameraOffset,
			{ PlayerCollider, PlayerCollider2 });
		if (GroundHit.Hit)
		{
			PlayerCamera->SetTransform(Transform(Vector3::Lerp(GroundHit.ImpactPoint - GetTransform().Location, Vector3(), 0.1), Rotation, Vector3(1)));
		}
		else
		{
			PlayerCamera->SetTransform(Transform(Vector3::Lerp(CameraOffset, Vector3(), 0.1), Rotation, Vector3(1)));
		}


		Time += Performance::DeltaTime;
	}
}

void PlayerObject::Begin()
{
	SetTransform(GetTransform() + Transform(Vector3(0, 15, 0), Vector3(0), Vector3(1)));
	SpawnPoint = GetTransform().Location;
	UI = UI::CreateUICanvas<GameUI>();
	if(UI)
	UI->Player = this;
	PlayerMesh = new MeshComponent();
	Attach(PlayerMesh);
	PlayerMesh->Load("Cube");
	PlayerMesh->SetRelativeTransform(Transform(Vector3(), Vector3(), Vector3(0.5, 2, 0.5)));
	PlayerCollider = new CollisionComponent();
	Attach(PlayerCollider);
	PlayerCollider->Init(PlayerMesh->GetMeshData().Vertices, PlayerMesh->GetMeshData().Indices, Transform(Vector3(), Vector3(), Vector3(0.3, 1.9, 0.3)));
	PlayerCollider2 = new CollisionComponent();
	Attach(PlayerCollider2);
	PlayerCollider2->Init(PlayerMesh->GetMeshData().Vertices, PlayerMesh->GetMeshData().Indices, Transform(Vector3(0, -0.5, 0), Vector3(), Vector3(0.2, 1.75, 0.2)));
	PlayerCamera = new CameraComponent();
	Attach(PlayerCamera);


	PlayerCamera->Use();
	if (UI)
	{
		UI->EndTransition();
	}
}

void PlayerObject::Destroy()
{
	delete PlayerCamera;
	delete PlayerCollider;
	delete UI;
}


bool PlayerObject::TryMove(Vector3 Offset, bool Vertical)
{
	Vector3 LastValidPosition;
	if (Offset.Length() != 0)
	{
		if (!Vertical)
		{
			auto hit = Collision::LineTrace(GetTransform().Location, GetTransform().Location + Vector3(0, -5, 0),
				{ PlayerCollider, PlayerCollider2 });
			float TerrainDif = (1 - abs(Vector3::Dot(Vector3(0, 1, 1), hit.Normal)));
			Offset = Offset - Vector3(0, Performance::DeltaTime * TerrainDif * 15, 0);
		}
		for (float i = 0; i < Offset.Length();)
		{
			i += 0.1f;

			if (i > Offset.Length())
				i = Offset.Length();
			Vector3 NextLastValidPosition = Offset * (i / Offset.Length());;
			PlayerCollider->RelativeTransform = Transform(NextLastValidPosition, Vector3(), Vector3(0.0075, 0.04, 0.0075));
			Collision::HitResponse hit = PlayerCollider->OverlapCheck({PlayerCollider2});

			if (hit.Hit && Vector3::Dot(hit.Normal, Vector3(0, 1, 0)) > 0.5)
			{
				ObjectTransform.Location += Vector3(0, Performance::DeltaTime * (1.5 - Vector3::Dot(hit.Normal, Vector3())) * 25, 0);
				return true;
			}
			else if (hit.Hit && Vector3::Dot(hit.Normal, Vector3(0, 1, 0)) < -0.5)
			{
				VerticalVelocity = -10;
				return false;
			}
			else if (hit.Hit)
			{
				ObjectTransform.Location += LastValidPosition;
				if(Vector3::Dot(hit.Normal, Vector3()) < 0.5)
				return false;
			}
			LastValidPosition = NextLastValidPosition;
		}
	}
	ObjectTransform.Location += LastValidPosition;
	return true;
}


void PlayerObject::Respawn()
{
	IsDead = false;
	ObjectTransform.Location = SpawnPoint;
	VerticalVelocity = 0;
}