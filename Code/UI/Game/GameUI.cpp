#include "GameUI.h"
#include <Objects/PlayerObject.h>

GameUI::GameUI()
{
	this->Player = Player;
}

GameUI::~GameUI()
{
	delete GameTextRenderer;
}

void GameUI::Render(Shader* Shader)
{
	int NumBombs = Player->BombTime / 0.2f;

	GameTextRenderer->RenderText("Bomb: " + std::to_string(NumBombs), Vector2(-0.8f), 1.f, Vector3(1));

	if (InTransition)
	{
		TransitionBorder->ReInit(Vector2(1 - std::min(std::pow(TransitionTime, 3.f), 2.f), -1.f), Vector2(2), 0);
		TransitionTime += Performance::DeltaTime * (TransitionDirection ? -3 : 3);
		if (TransitionTime > 2)
		{
			EndTransition();
			Player->Respawn();
		}
		if (TransitionTime < 0)
		{
			InTransition = false;
		}
	}
	TransitionBorder->Render(Shader);
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