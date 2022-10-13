#include "Sound.h"
#include <AL/al.h>
#include <FileUtility.h>
#include <AL/alc.h>
#include "AL/alext.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <Log.h>
#include <Rendering/Camera/Camera.h>
#include <World/Assets.h>
#include <iostream>
#include <World/Graphics.h>
struct Source
{
	Source(ALuint AudioSource, float Pitch, float Volume, bool Looping, bool Is3D, Vector3 Location, float Distance, std::string Name)
	{
		this->AudioSource = AudioSource;
		this->Pitch = Pitch;
		this->Volume = Volume;
		this->Looping = Looping;
		this->Is3D = Is3D;
		this->Location = Location;
		this->Distance = Distance;
		this->name = Name;
	}
	float Pitch = 1.f;
	float Volume = 1.f;
	bool Looping = false;
	bool Is3D = false;
	Vector3 Location;
	float Distance;
	ALuint AudioSource;
	std::string name;
};
std::vector<Source> CurrentSources;
class SoundException : public std::exception
{
	std::string Message;
public:
	SoundException(std::string Message)
	{
		this->Message = Message;
	}

	const char* what() const throw ()
	{
		return ("SoundException: " + Message).c_str();
	}
};
std::vector<ALuint> Buffers;
ALCdevice* CurrentDevice;
ALCcontext* ALContext;
std::vector<std::string> GetAvaliableDevices(ALCdevice* device)
{
	const ALCchar* devices = alcGetString(device, ALC_ALL_DEVICES_SPECIFIER);

	const char* ptr = devices;

	std::vector<std::string> devicesVec;

	do
	{
		devicesVec.push_back(std::string(ptr));
		ptr += devicesVec.back().size() + 1;
	} while (*(ptr + 1) != '\0');

	return devicesVec;
}
int InitAL(char*** argv, int* argc)
{
	const ALCchar* name;
	CurrentDevice;

	/* Open and initialize a device */
	CurrentDevice = NULL;
	if (argc && argv && *argc > 1 && strcmp((*argv)[0], "-device") == 0)
	{
		CurrentDevice = alcOpenDevice((*argv)[1]);
		if (!CurrentDevice)
			fprintf(stderr, "Failed to open \"%s\", trying default\n", (*argv)[1]);
		(*argv) += 2;
		(*argc) -= 2;
	}
	if (!CurrentDevice)
		CurrentDevice = alcOpenDevice(NULL);
	if (!CurrentDevice)
	{
		fprintf(stderr, "Could not open a device!\n");
		return 1;
	}

	ALContext = alcCreateContext(CurrentDevice, NULL);
	if (ALContext == NULL || alcMakeContextCurrent(ALContext) == ALC_FALSE)
	{
		if (ALContext != NULL)
			alcDestroyContext(ALContext);
		alcCloseDevice(CurrentDevice);
		fprintf(stderr, "Could not set a context!\n");
		return 1;
	}

	name = NULL;
	if (alcIsExtensionPresent(CurrentDevice, "ALC_ENUMERATE_ALL_EXT"))
		name = alcGetString(CurrentDevice, ALC_ALL_DEVICES_SPECIFIER);
	if (!name || alcGetError(CurrentDevice) != AL_NO_ERROR)
		name = alcGetString(CurrentDevice, ALC_DEVICE_SPECIFIER);
	printf("Opened \"%s\"\n", name);

	return 0;
}
bool isBigEndian()
{
	int a = 1;
	return !((char*)&a)[0];
}


static void printDeviceList(const char* list)
{
	if (!list || *list == '\0')
		printf("    !!! none !!!\n");
	else do {
		printf("    %s\n", list);
		list += strlen(list) + 1;
	} while (*list != '\0');
}
int convertToInt(char* buffer, int len)
{
	int a = 0;
	if (!isBigEndian())
		for (int i = 0; i < len; i++)
			((char*)&a)[i] = buffer[i];
	else
		for (int i = 0; i < len; i++)
			((char*)&a)[3 - i] = buffer[i];
	return a;
}

char* loadWAV(const char* fn, int& chan, int& samplerate, int& bps, int& size)
{
	char buffer[4];
	std::ifstream in(fn, std::ios::binary);
	in.read(buffer, 4);
	if (strncmp(buffer, "RIFF", 4) != 0)
	{
		std::cout << "this is not a valid WAVE file (" << fn << ")" << std::endl;
		return NULL;
	}
	in.read(buffer, 4);
	in.read(buffer, 4);      //WAVE
	in.read(buffer, 4);      //fmt
	in.read(buffer, 4);      //16
	in.read(buffer, 2);      //1
	in.read(buffer, 2);
	chan = convertToInt(buffer, 2);
	in.read(buffer, 4);
	samplerate = convertToInt(buffer, 4);
	in.read(buffer, 4);
	in.read(buffer, 2);
	in.read(buffer, 2);
	bps = convertToInt(buffer, 2);
	in.read(buffer, 4);      //data
	in.read(buffer, 4);
	size = convertToInt(buffer, 4);
	char* data = new char[size];
	in.read(data, size);
	return data;
}

namespace Sound
{
	void StopAllSounds()
	{
		for (Source s : CurrentSources)
		{
			if (alIsSource(s.AudioSource))
			{
				alDeleteSources(1, &s.AudioSource);
			}
		}
		CurrentSources.clear();
	}
	void Update()
	{
		Vector3 ForwardVec, PositionVec;
		if (Graphics::MainCamera)
		{
			ForwardVec = Vector3::GetForwardVector(Graphics::MainCamera->Rotation);
			PositionVec = Graphics::MainCamera->Position;
		}
		else
		{
			ForwardVec = Vector3(0, 0, 1);
		}
		Vector3 Forward[2] = { ForwardVec, Vector3(0, 1, 0)};
		alListenerfv(AL_POSITION, &PositionVec.X);
		alListenerfv(AL_ORIENTATION, &Forward[0].X);
		for (int i = 0; i < CurrentSources.size(); i++)
		{
			ALenum SoundStatus;
			alGetSourcei(CurrentSources[i].AudioSource, AL_SOURCE_STATE, &SoundStatus);
			if (SoundStatus != AL_PLAYING)
			{
				alDeleteSources(1, &CurrentSources[i].AudioSource);
				CurrentSources.erase(CurrentSources.begin() + i);
			}
			else if (CurrentSources[i].Is3D)
			{
				alSourcef(CurrentSources[i].AudioSource, AL_GAIN, std::min(1 / (CurrentSources[i].Location
					- Vector3::Vec3ToVector(PositionVec)).Length() * CurrentSources[i].Distance, 1.f) * CurrentSources[i].Volume);
			}
		}
	}
	void Init()
	{
		ALCdevice* device = alcOpenDevice(NULL);
		if (device == NULL)
		{
			throw SoundException("Cannot open sound card");
		}
		ALCcontext* context = alcCreateContext(device, NULL);
		if (context == NULL)
		{
			throw SoundException("Cannot open Context");
		}
		alcMakeContextCurrent(context);
		alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
		alListenerf(AL_GAIN, 1.1f);
	}
	void End()
	{
		alcCloseDevice(CurrentDevice);
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(ALContext);
		Log::CreateNewLogMessage("Sound: Shutting down", Vector3(1, 1, 0));
	}
	std::string GetVersionString()
	{
		return alGetString(AL_VERSION);
	}
	std::vector<std::string> GetSounds()
	{
		std::vector<std::string> Sounds;
		for (Source& s : CurrentSources)
		{
			if (s.Is3D)
			{
				Sounds.push_back(s.name + " (Is 3D, distance = " + std::to_string(s.Distance) + ")");
			}
			else
			{
				Sounds.push_back(s.name);
			}
		}
		return Sounds;
	}
	SoundSource PlaySound3D(SoundBuffer* Sound, Vector3 Position, float MaxDistance, float Pitch, float Volume, bool Looping)
	{
		ALuint NewSource;
		alGenSources(1, &NewSource);
		alSourcef(NewSource, AL_PITCH, Pitch);
		alSourcef(NewSource, AL_GAIN, Volume);
		alSource3f(NewSource, AL_POSITION, 0, 0, 0);
		alSource3f(NewSource, AL_POSITION, Position.X, Position.Y, Position.Z);
		alSource3f(NewSource, AL_VELOCITY, 0, 0, 0);
		alSourcef(NewSource, AL_MAX_DISTANCE, MaxDistance);
		alSourcei(NewSource, AL_LOOPING, Looping);
		alSourcei(NewSource, AL_BUFFER, Sound->Buffer);
		alSourcePlay(NewSource);
		CurrentSources.push_back(Source(NewSource, Pitch, Volume, Looping, true, Position, MaxDistance, Sound->Name + " (" + std::to_string(Sound->Buffer) + "/255)"));
		return SoundSource(NewSource, Pitch, Volume, Looping);
	}

	SoundSource PlaySound2D(SoundBuffer* Sound, float Pitch, float Volume, bool Looping)
	{
		ALuint NewSource;
		alGenSources(1, &NewSource);
		alSourcef(NewSource, AL_PITCH, Pitch);
		alSourcef(NewSource, AL_GAIN, Volume);
		alSourcei(NewSource, AL_SOURCE_RELATIVE, 1);
		alSource3f(NewSource, AL_POSITION, 0, 0, 0);
		alSourcei(NewSource, AL_LOOPING, Looping);
		alSourcei(NewSource, AL_BUFFER, Sound->Buffer);
		alSourcePlay(NewSource);
		CurrentSources.push_back(Source(NewSource, Pitch, Volume, Looping, false, Vector3(), 0, Sound->Name + " (" + std::to_string(Sound->Buffer) + "/255)"));
		return SoundSource(NewSource, Pitch, Volume, Looping);
	}

	SoundBuffer* LoadSound(std::string File)
	{
		File.append(".wav");
		int channel, sampleRate, bps, size;
		char* data = loadWAV(Assets::GetAsset(File).c_str(), channel, sampleRate, bps, size);
		
		unsigned int bufferid, format;
		alGenBuffers(1, &bufferid);
		if (channel == 1)
		{
			if (bps == 8)
			{
				format = AL_FORMAT_MONO8;
			}
			else {
				format = AL_FORMAT_MONO16;
			}
		}
		else {
			if (bps == 8)
			{
				format = AL_FORMAT_STEREO8;
			}
			else {
				format = AL_FORMAT_STEREO16;
			}
		}
		alBufferData(bufferid, format, data, size, sampleRate);
		return new SoundBuffer(bufferid, File);
	}
	SoundBuffer::~SoundBuffer()
	{
		alDeleteBuffers(1, &Buffer);
	}
	SoundSource::SoundSource(unsigned int Buffer, float Pitch, float Volume, bool Looping)
	{
		this->Source = Buffer;
		this->Pitch = Pitch;
		this->Volume = Volume;
		this->Looping = Looping;
	}
	void SoundSource::Stop()
	{
		alSourceStop(Source);
		alDeleteSources(1, &Source);
		for (::Source s : CurrentSources)
		{
			if (s.AudioSource == Source)
			{
				alDeleteSources(1, &s.AudioSource);
			}
		}
	}
	void SoundSource::SetPitch(float NewPitch)
	{
		if(alIsSource(Source))
		alSourcef(Source, AL_PITCH, NewPitch);
	}
	void SoundSource::SetVolume(float NewVolume)
	{
		if (alIsSource(Source))
		alSourcef(Source, AL_GAIN, NewVolume);
	}
	float SoundSource::GetPitch()
	{
		return Pitch;
	}
	float SoundSource::GetVolume()
	{
		return Volume;
	}
	bool SoundSource::GetLooping()
	{
		return Looping;
	}
}