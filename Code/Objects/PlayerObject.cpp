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
#include <Objects/WallObject.h>
#include <Objects/Orb.h>
#include <Objects/HubTeleporter.h>
#include <Objects/BombPickup.h>

#include <FileUtility.h>

Sound::SoundBuffer* BombPlaceSound = nullptr;
Sound::SoundBuffer* OrbSound = nullptr;

std::string NextLevel;

struct Animation { unsigned int Frames; unsigned int StartingFrame; };
Animation Anims[4] =
{
	{0, 0},
	{3, 1},
	{0, 5},
	{3, 6}
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

float CameraDistance = 35;
float Gravity = 200;
float Speed = 750;
void LoadNextLevel()
{
	World::ScheduleLoadNewScene(NextLevel);
}

void PlayerObject::TryLoadSave()
{
	if (!IsInEditor)
	{
		try
		{
			//first load the main save game and get the data that is the same no matter what level we're in
			SaveGame PersistantSave = SaveGame("Main");
			NumOrbs = std::stoi(PersistantSave.GetPropterty("OrbsCollected").Value);

			//then load the Level save game and load level relevant data
			SaveGame InSave = SaveGame(GetFileNameWithoutExtensionFromPath(CurrentScene));
			if(!InSave.SaveGameIsNew())
			{
				int NumWalls = std::stoi(InSave.GetPropterty("NumWalls").Value);
				int NumOrbs = std::stoi(InSave.GetPropterty("NumOrbs").Value);
				auto Walls = Objects::GetAllObjectsWithID(6); // Get All Walls

				for (auto* w : Walls) // destroy all walls, because we are going to spawn new ones that are listed in the savegame
				{
					Objects::DestroyObject(w);
				}
				auto Orbs = Objects::GetAllObjectsWithID(8); // then do the same with the orbs

				for (auto* o : Orbs)
				{
					Objects::DestroyObject(o);
				}

				GetTransform().Location = Vector3::stov(InSave.GetPropterty("PlayerPos").Value);
				SpawnPoint = GetTransform().Location;
				TeleportCancelTime = 1.0f;
				for (int i = 0; i < NumWalls; i++)
				{
					//spawn new walls that are listed in the save game
					Vector3 Loc = Vector3::stov(InSave.GetPropterty("Walls_pos" + std::to_string(i)).Value);
					Vector3 Rot = Vector3::stov(InSave.GetPropterty("Walls_rot" + std::to_string(i)).Value);
					int HasOrb = std::stoi(InSave.GetPropterty("Walls_orb" + std::to_string(i)).Value);
					Objects::SpawnObject<WallObject>(Transform(Loc, Rot, Vector3(1)))->ContainsOrb = HasOrb;
				}

				for (int i = 0; i < NumOrbs; i++)
				{
					//spawn new orbs that are listed in the save game
					Vector3 Loc = Vector3::stov(InSave.GetPropterty("Orbs_pos" + std::to_string(i)).Value);
					Objects::SpawnObject<Orb>(Transform(Loc, Vector3(), Vector3(1)));
				}
			}
		}
		catch (std::exception) // if we get an error when loading, that probably means the save file does not exist, so we treat this as a "new game"
		{
		}
	}
}

void PlayerObject::Tick()
{
	if (End) return;
	if (!InLevelTransition)
	{
		if (!LoadedSave)
		{
			TryLoadSave();
			LoadedSave = true;
		}

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


			if (BombTime > 0.1f && BombLayTime < 0 && BombTime < MaxBombs - 0.1f)
			{
				BombLayTime = 0.5f;
				Objects::SpawnObject<Bomb>(GetTransform() + Transform(Vector3(), Vector3(0, Random::GetRandomNumber(-100, 100), 0), Vector3(1)));
				Sound::PlaySound2D(BombPlaceSound, 1, 0.1f);
			}

			BombLayTime -= Performance::DeltaTime;
			BombTime -= Performance::DeltaTime;

			BombTime = std::max(0.0f, BombTime);

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

			if (TeleportCancelTime > 0)
			{
				TeleportCancelTime -= Performance::DeltaTime;
			}

			if (Health < 0)
			{
				if (!IsDead)
				{
					CameraShake::PlayDefaultCameraShake(3);
					UI->PlayTransition();
					IsDead = true;
					Health = 100.0f;
					BombTime = 0;

				}
			}
			else
			{
				Health = std::min(Health + Performance::DeltaTime * 25, 100.0f);
			}

			if (GetTransform().Location.Y < -1000.f && !IsDead)
			{
				CameraShake::PlayDefaultCameraShake(3);
				UI->PlayTransition();
				IsDead = true;

			}


			Rotation += Vector3(Vector2(Input::MouseMovement.Y, Input::MouseMovement.X), 0) * Performance::DeltaTime * 275;
			//Clamp the camera rotation so the camera stays somewhat top-down-ish;
			Rotation = Vector3(Rotation.X = std::max(std::min(Rotation.X, -35.f), -85.f), Rotation.Y, Rotation.Z);

			Vector3 CameraOffset = (PrevCameraPos * (1 - Performance::DeltaTime * 5)
				+ (Vector3::GetForwardVector(Rotation) * -CameraDistance) * (Performance::DeltaTime * 5)) + Vector3(0, Performance::DeltaTime * 25, 0);


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

			AnimFrameTimer += Performance::DeltaTime;
			
			//Chose what animation we're playing right now

			//if we still have bombs, we should choose an animation that has them
			unsigned int AnimOffset = BombTime > 0.5 ? 2 : 0;

			CurrentAnim = (Vector3(Velocity, 0).Length() > 15) + AnimOffset;


			//if the timer reaches 0.1f, we trigger an animation change
			if (AnimFrameTimer > 0.1f)
			{
				CurrentAnimFrame++;

				//Loop the animation
				if (CurrentAnimFrame > Anims[CurrentAnim].StartingFrame + Anims[CurrentAnim].Frames)
				{
					CurrentAnimFrame = Anims[CurrentAnim].StartingFrame;
				}


				for (unsigned int i = 0; i < PLAYER_NUM_ANIM_FRAMES; i++)
				{
					AllAnimComponents[i]->SetVisibility(i == CurrentAnimFrame);

					if (Vector3(Velocity.Y, 0, Velocity.X).Length() > 15)
					{
						AllAnimComponents[i]->SetRelativeTransform(Transform(Vector3(0, -4, 0),
							Vector3::LookAtFunction(Vector3(), Vector3(Velocity.Y, 0, Velocity.X), true) + Vector3(0, M_PI, 0),
							Vector3(1, 1, 1)));
					}

				}
				if ((CurrentAnimFrame) % 2 && AllAnimMeshes[CurrentAnimFrame].substr(0, 4) != "Idle")
				{
					Sound::PlaySound2D(Footstepsounds, Random::GetRandomNumber(1.1f, 1.5f), 0.02, false);
				}
				AnimFrameTimer = 0;
			}
			Time += Performance::DeltaTime;
		}
	}
}

void PlayerObject::Begin()
{
	GetTransform().Rotation = 0;
	SpawnPoint = GetTransform().Location;
	UI = UI::CreateUICanvas<GameUI>();
	if (UI)
		UI->Player = this;

	auto CollisionMeshData = JSM::LoadJSModel(Assets::GetAsset("Cube.jsm"));

	PlayerCollider = new CollisionComponent();
	Attach(PlayerCollider);
	PlayerCollider->Init(CollisionMeshData.Vertices[0], CollisionMeshData.Indices[0], Transform(Vector3(), Vector3(), Vector3(0.3, 1.9, 0.3)));

	PlayerCollider2 = new CollisionComponent();
	Attach(PlayerCollider2);
	PlayerCollider2->Init(CollisionMeshData.Vertices[0], CollisionMeshData.Indices[0], Transform(Vector3(0, -0.5, 0), Vector3(), Vector3(0.2, 1.75, 0.2)));

	PlayerCamera = new CameraComponent();
	Attach(PlayerCamera);

	for (unsigned int i = 0; i < PLAYER_NUM_ANIM_FRAMES; i++)
	{
		auto NewAnimMesh = new MeshComponent();
		Attach(NewAnimMesh);
		NewAnimMesh->Load(AllAnimMeshes[i]);
		NewAnimMesh->SetRelativeTransform(Transform(Vector3(0, -4, 0), 0, 1));
		AllAnimComponents[i] = NewAnimMesh;
	}

	if (!BombPlaceSound)
	{
		BombPlaceSound = Sound::LoadSound("PlaceBomb");
		OrbSound = Sound::LoadSound("Orb");
	}

	PlayerCamera->Use();
	if (UI)
	{
		UI->EndTransition();
	}

}

void PlayerObject::Destroy()
{
	if (!IsInEditor)
	{
		{
			//Save general information into the main save
			SaveGame PersistantSave = SaveGame("Main");
			PersistantSave.SetPropterty(SaveGame::SaveProperty("OrbsCollected", std::to_string(NumOrbs), T_INT));
			PersistantSave.SetPropterty(SaveGame::SaveProperty("CurrentMap", GetFileNameWithoutExtensionFromPath(CurrentScene), T_STRING));
		}

		//Then open a save for the current level
		SaveGame OutSave = SaveGame(GetFileNameWithoutExtensionFromPath(CurrentScene));
		OutSave.ClearProperties();

		//Save the Player's position
		OutSave.SetPropterty(SaveGame::SaveProperty("PlayerPos", GetTransform().Location.ToString(), T_VECTOR3));

		int i = 0;
		auto Walls = Objects::GetAllObjectsWithID(6); //Get all Walls
		auto Orbs = Objects::GetAllObjectsWithID(8); //Get all Orbs

		//then save the ammount of both into the level save file
		OutSave.SetPropterty(SaveGame::SaveProperty("NumWalls", std::to_string(Walls.size()), T_INT));
		OutSave.SetPropterty(SaveGame::SaveProperty("NumOrbs", std::to_string(Orbs.size()), T_INT));
		for (auto* w : Walls)
		{
			//Save all relevant information about each wall into the save file
			OutSave.SetPropterty(SaveGame::SaveProperty("Walls_pos" + std::to_string(i), w->GetTransform().Location.ToString(), T_VECTOR3));
			OutSave.SetPropterty(SaveGame::SaveProperty("Walls_rot" + std::to_string(i), w->GetTransform().Rotation.ToString(), T_VECTOR3));
			OutSave.SetPropterty(SaveGame::SaveProperty("Walls_orb" + std::to_string(i), std::to_string(dynamic_cast<WallObject*>(w)->ContainsOrb), T_INT));
			i++;
		}
		i = 0;
		for (auto* o : Orbs)
		{
			//Save all relevant information about each orb into the save file
			OutSave.SetPropterty(SaveGame::SaveProperty("Orbs_pos" + std::to_string(i), o->GetTransform().Location.ToString(), T_VECTOR3));

			i++;
		}
	}
	delete Footstepsounds;
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

			if (hit.Hit)
			{
				if (hit.HitObject->GetObjectDescription().ID == 7) // if its a bomb pickup
				{
					BombTime = dynamic_cast<BombPickup*>(hit.HitObject)->Amount;
					MaxBombs = BombTime;
				}
				else if (hit.HitObject->GetObjectDescription().ID == 8) // if its an orb
				{
					NumOrbs++;
					Objects::DestroyObject(hit.HitObject);
					Sound::PlaySound2D(OrbSound, 1.f, 0.2f);
				}
				else if (hit.HitObject->GetObjectDescription().ID == 9 && !InLevelTransition) // if its a teleporter
				{
					if (TeleportCancelTime <= 0)
					{
						InLevelTransition = true;
						NextLevel = ((HubTeleporter*)hit.HitObject)->TargetLevel;
						Timer::StartTimer(LoadNextLevel, 0.6);
						UI->PlayTransition();
					}
					else
					{
						TeleportCancelTime = 3.f;
					}
				}
				else if (hit.HitObject->GetObjectDescription().ID == 12)
				{
					if (TeleportCancelTime <= 0)
					{
						UI->PlayEndCutscene();
						End = true;
					}
				}
				else if (Vector3::Dot(hit.Normal, Vector3(0, 1, 0)) > 0.5)
				{
					ObjectTransform.Location += Vector3(0, Performance::DeltaTime * (1.5 - Vector3::Dot(hit.Normal, Vector3(0, 1, 0))) * 35, 0);
					return true;
				}
				else if (Vector3::Dot(hit.Normal, Vector3(0, 1, 0)) < -0.5)
				{
					VerticalVelocity = -10;
					return false;
				}
				else
				{

					if (std::to_string(hit.Normal.X) == "-nan(ind)")
					{
						GetTransform().Location = hit.ImpactPoint;
					}
					if (abs(Vector3::Dot(hit.Normal, Vector3(0, 1, 0))) < 0.5)
					{
						GetTransform().Location += hit.Normal * Performance::DeltaTime * 2;
						return false;
					}
				}
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