#pragma once

#include "IAudioFileLoader.h"

#include <AL/al.h>
#include <memory>

struct AudioStream
{
	AudioStream(size_t bufferCount, size_t bufferSize, std::unique_ptr<IAudioFileLoader> loader);
	~AudioStream();

	void LoadStreamData();

	ALuint *buffers;
	size_t bufferCount;
	size_t bufferSize;
	ALenum format = AL_FORMAT_MONO16;
	std::unique_ptr<IAudioFileLoader> loader;
};