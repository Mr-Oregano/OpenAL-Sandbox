
#include "AudioStream.h"
#include "AudioDevice.h"

#include <iostream>
#include <utility>

AudioStream::AudioStream(size_t bufferCount, size_t bufferSize, std::unique_ptr<IAudioFileLoader> loader)
	: bufferCount(bufferCount), bufferSize(bufferSize), loader(std::move(loader))
{
    switch (this->loader->GetChannels())
    {
        case 1: format = AL_FORMAT_MONO16; break;
        case 2: format = AL_FORMAT_STEREO16; break;
        default: std::cerr << "Unknown audio format" << std::endl;
    }

    buffers = new ALuint[bufferCount];
    AL_CALL(alGenBuffers((ALsizei)bufferCount, buffers));

    LoadStreamData();
}

AudioStream::~AudioStream()
{
    AL_CALL(alDeleteBuffers((ALsizei) bufferCount, buffers));
    delete[] buffers;
}

void AudioStream::LoadStreamData()
{
    unsigned char *data = new unsigned char[bufferSize] { 0 };
    for (int i = 0; i < bufferCount; ++i)
    {
        size_t framesToRead = bufferSize / loader->GetFrameSize();
        size_t framesRead = loader->ReadFrames(framesToRead, data);
        AL_CALL(alBufferData(buffers[i], format, data, (ALsizei) (framesRead * loader->GetFrameSize()), (ALsizei) loader->GetSampleRate()));
    }
    delete[] data;
}
