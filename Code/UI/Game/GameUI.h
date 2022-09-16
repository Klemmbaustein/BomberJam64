#pragma once
#include <UI/Default/UICanvas.h>
#include <UI/Default/TextRenderer.h>
#include <Sound/Sound.h>
class PlayerObject;

class GameUI : public UICanvas
{
public:
	GameUI();
	~GameUI();
	void Render(Shader* Shader) override;
	PlayerObject* Player;

	void PlayTransition();
	void EndTransition();

	void PlayEndCutscene();

protected:
	unsigned int Icons[2];
	Sound::SoundBuffer* TextSound = Sound::LoadSound("Walk");

	bool InEndCutscene = false;
	bool InTransition = false;
	bool TransitionDirection = false;
	float TransitionTime = 0;
	UIBorder* HealthBorder = new UIBorder(Vector2(-1, -1), Vector2(2), Vector3(1, 0, 0));
	UIBorder* TransitionBorder = new UIBorder(Vector2(1, -1), Vector2(2), 0);
	TextRenderer* GameTextRenderer = new TextRenderer("Fonts/Font.ttf", 100);
};