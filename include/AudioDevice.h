#pragma once

#include "AudioStreamPlayer.h"
#include <AL/al.h>

#ifdef DEBUG
	#define AL_CALL(func) do { func; if (alGetError() != AL_NO_ERROR) { __debugbreak(); } } while(false)
#else
	#define AL_CALL(func) func
#endif

class AudioDevice
{
public:
	static void Init();
	static void Shutdown();

	static void RegisterStream(AudioStreamPlayer *streamer);
	static void UnregisterStream(AudioStreamPlayer *streamer);
	static void UpdateStreams();
};