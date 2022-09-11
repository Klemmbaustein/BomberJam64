#include "GameUI.h"
#include <Objects/PlayerObject.h>
#include <Rendering/Texture/Texture.h>

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

void GameUI::Render(Shader* Shader)
{
	int NumBombs = Player->BombTime / 0.5f;

	GameTextRenderer->RenderText("x" + std::to_string(NumBombs), Vector2(-0.81f, -0.8f), 1.2f, Vector3(1));
	GameTextRenderer->RenderText("x" + std::to_string(Player->NumOrbs), Vector2(-0.81f, -0.5f), 1.2f, Vector3(1));
	HealthBorder->Opacity = 1 - (Player->Health / 200.0f + 0.5f);
	HealthBorder->Render(Shader);
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