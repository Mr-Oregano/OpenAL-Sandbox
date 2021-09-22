
#include "IAudioPlayer.h"
#include "IAudioFileLoader.h"
#include "AudioDevice.h"

#include "AudioBufferPlayer.h"
#include "AudioStreamPlayer.h"

#include <iostream>

AudioState IAudioPlayer::PollState() const
{
    ALenum state;
    AL_CALL(alGetSourcei(m_SourceID, AL_SOURCE_STATE, &state));

    switch (state)
	{
        case AL_PLAYING: return AudioState::Playing;
	    case AL_PAUSED: return AudioState::Paused;
	    case AL_STOPPED: return AudioState::Stopped;
	}

    return AudioState::Stopped;
}

std::shared_ptr<IAudioPlayer> IAudioPlayer::LoadFromFile(const std::string &path, const AudioProps &props)
{
    std::unique_ptr<IAudioFileLoader> loader = std::make_unique<IAudioFileLoader>(path);

    if (!loader)
    {
        std::cerr << "Failed to open file: " << path << std::endl;
        return nullptr;
    }

    size_t threshold = props.streamBufferCount * props.streamBufferSize;
    if (loader->GetTotalSize() > threshold) // Stream mode will be enabled
    {
        std::cout << "The file " << path << " will be played in stream mode" << std::endl;
        return std::make_shared<AudioStreamPlayer>(props, std::move(loader));
    }

    std::cout << "The file " << path << " will be played in buffer mode" << std::endl;
    return std::make_shared<AudioBufferPlayer>(props, *loader);

}
