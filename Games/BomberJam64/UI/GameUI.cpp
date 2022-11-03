#include "GameUI.h"
#include <Objects/PlayerObject.h>
#include <Rendering/Texture/Texture.h>
#include <Engine/EngineRandom.h>

#include <Objects/Objects.h>
#include <World/Stats.h>

namespace Application
{
	void Quit();
}

GameUI::GameUI()
{
	Icons[0] = Texture::LoadTexture("BombIcon.png");
	Icons[1] = Texture::LoadTexture("Orb.png");
	this->Player = Player;
	UIElements.push_back(new UIBorder(Vector2(-0.8f, -0.6f), Vector2(-2.5f) / Vector2(16, 9), Vector3(1), Icons[0], true));
	UIElements.push_back(new UIBorder(Vector2(-0.8f, -0.3f), Vector2(-2.5f) / Vector2(16, 9), Vector3(1), Icons[1], true));

}

GameUI::~GameUI()
{
	delete GameTextRenderer;
}

constexpr unsigned int NUM_CUTSCENE_STRINGS = 5;
unsigned int CurrentEndLetter = 0;
unsigned int CurrentEndLine = 0;

std::string EndCutsceneStrings[NUM_CUTSCENE_STRINGS] =
{
	"You have found a new backpack!",
	"Now you will no longer have to drop bombs.",
	"That makes you happy.",
	"The End.",
};

bool CutsceneIsEnd = false;

float EndTimer = 0.0f;

void GameUI::Render(Shader* Shader)
{
	int NumBombs = Player->BombTime / 0.5f;

	GameTextRenderer->RenderText("x" + std::to_string(NumBombs), Vector2(-0.81f, -0.8f), 1.2f, Vector3(1));
	GameTextRenderer->RenderText("x" + std::to_string(Player->NumOrbs), Vector2(-0.81f, -0.5f), 1.2f, Vector3(1));
	HealthBorder->Opacity = 1 - (Player->Health / 200.0f + 0.5f);
	HealthBorder->Render(Shader);
	TransitionBorder->Render(Shader);

	if (InTransition)
	{
		TransitionBorder->ReInit(Vector2(1 - std::min(std::pow(TransitionTime, 3.f), 2.f), -1.f), Vector2(2), 0);
		TransitionTime += Performance::DeltaTime * (TransitionDirection ? -3 : 3);
		if (TransitionTime > 2)
		{
			for (auto* b : Objects::GetAllObjectsWithID(5))
			{
				Objects::DestroyObject(b);
			}
			if (!InCutscene)
			{
				EndTransition();
				Player->Respawn();
			}
			else
			{
				if (CutsceneIsEnd)
				{
					EndTimer += Performance::DeltaTime;
					if (EndTimer > 0.1f && EndCutsceneStrings[CurrentEndLine].length() > CurrentEndLetter)
					{
						CurrentEndLetter++;
						EndTimer = 0.0f;
						if (EndCutsceneStrings[CurrentEndLine][CurrentEndLetter] != ' ')
							Sound::PlaySound2D(TextSound, Random::GetRandomNumber(1.1f, 1.5f), 0.02f);
					}
					else if (EndTimer > 0.5f && CurrentEndLine < NUM_CUTSCENE_STRINGS)
					{
						CurrentEndLine++;
						CurrentEndLetter = 0;
						EndTimer = 0.0f;
					}
					else if (EndTimer > 5)
					{
						Application::Quit();
					}
					for (unsigned int i = 0; i < NUM_CUTSCENE_STRINGS; i++)
					{
						if (i <= CurrentEndLine)
						{
							if (i == CurrentEndLine)
							{
								GameTextRenderer->RenderText(EndCutsceneStrings[i].substr(0, CurrentEndLetter), Vector2(-0.5f, 0.25f - (i / 8.f)), 0.9f, 1.f);
							}
							else
							{
								GameTextRenderer->RenderText(EndCutsceneStrings[i], Vector2(-0.5f, 0.25f - (i / 8.f)), 0.9f, 1.f);
							}
						}
					}
				}
				else
				{
					Player->BombTime = 0;
					GameTextRenderer->RenderText("Do not destroy the snowmen", Vector2(-0.5f, 0.2f), 1.5f, 1.f);
					if (TransitionTime > 7)
					{
						EndTransition();
						Player->Respawn();
						InCutscene = false;
					}
				}
			}
		}
		if (TransitionTime < 0)
		{
			InTransition = false;
		}
	}
}

void GameUI::PlayTransition()
{
	TransitionTime = 0;
	InTransition = true;
	TransitionDirection = false;
}

void GameUI::EndTransition()
{
	TransitionTime = 2;
	InTransition = true;
	TransitionDirection = true;
}

void GameUI::PlayEndCutscene()
{
	if (!InCutscene)
	{
		CutsceneIsEnd = true;
		InCutscene = true;
		PlayTransition();
	}
}

void GameUI::RespawnSnowman()
{
	if (!InCutscene)
	{
		InCutscene = true;
		CutsceneIsEnd = false;
		PlayTransition();
	}
}