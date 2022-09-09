#pragma once
#include <Objects/WorldObject.h>
#include <Sound/Sound.h>
class SoundObject : public WorldObject
{
public:
	SoundObject() : WorldObject(ObjectDescription("Sound Object", 2)) {}
	virtual void Begin() override;
	virtual void Tick() override;
	virtual void OnPropertySet() override;
	virtual void Destroy() override;
protected:
	std::string Filename;
	float Pitch = 1, Volume = 1, FalloffRange = 10;
	Sound::SoundBuffer* Buffer = nullptr;
	Sound::SoundSource Source;
};