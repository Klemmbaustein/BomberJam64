#include "SoundObject.h"
#include <iostream>
void SoundObject::Begin()
{
	Properties.push_back(Object::Property("Sound", T_STRING, &Filename));
	Properties.push_back(Object::Property("Pitch", T_FLOAT, &Pitch));
	Properties.push_back(Object::Property("Volume", T_FLOAT, &Volume));
	Properties.push_back(Object::Property("Falloff Range", T_FLOAT, &FalloffRange));
}

void SoundObject::Tick()
{
}

void SoundObject::OnPropertySet()
{
	if (Buffer != nullptr)
	{
		delete Buffer;
		Source.Stop();
	}
	Buffer = Sound::LoadSound(Filename);
	Source = Sound::PlaySound3D(Buffer, ObjectTransform.Location, FalloffRange, Pitch, Volume, true);
}

void SoundObject::Destroy()
{
	if (Buffer != nullptr)
		delete Buffer;
}
