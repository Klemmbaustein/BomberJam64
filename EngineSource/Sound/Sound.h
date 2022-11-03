#pragma once
#include <string>
#include <Math/Vector.h>
namespace Sound
{
	struct SoundSource
	{
		SoundSource(unsigned int Buffer, float Pitch, float Volume, bool Looping);
		SoundSource()
		{

		}
		void Stop();
		void SetPitch(float NewPitch);
		void SetVolume(float NewVolume);
		float GetPitch();
		float GetVolume();
		bool GetLooping();
	private:
		unsigned int Source = 0;
		float Pitch = 0;
		float Volume = 0;
		bool Looping = 0;
	};

	struct SoundBuffer
	{
		SoundBuffer(unsigned int Buffer, std::string Name)
		{
			this->Buffer = Buffer;
			this->Name = Name;
		}
		~SoundBuffer();
		unsigned int Buffer;
		std::string Name;
	};
	void StopAllSounds();
	void Update();
	void Init();
	void End();
	std::string GetVersionString();
	std::vector<std::string> GetSounds();
	SoundSource PlaySound3D(SoundBuffer* Sound, Vector3 Position, float MaxDistance, float Pitch = 1.f, float Volume = 1.f, bool Looping = false);

	SoundSource PlaySound2D(SoundBuffer* Sound, float Pitch = 1.f, float Volume = 1.f, bool Looping = false);

	SoundBuffer* LoadSound(std::string File);
}