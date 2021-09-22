#pragma once

#include "IAudioPlayer.h"
#include "AudioStream.h"

#include <atomic>
#include <memory>

class AudioStreamPlayer : public IAudioPlayer
{
public:
	AudioStreamPlayer(const AudioProps &props, std::unique_ptr<IAudioFileLoader> loader);
	virtual ~AudioStreamPlayer();

	virtual void Play() override;
	virtual void Stop() override;
	virtual void Pause() override;

	// Required for streaming audio data
	//
	void UpdateStream();
	void ResetStream();

private:
	AudioStream m_Stream;
	unsigned char *m_StreamCache;
	std::atomic_bool m_Streaming = false;
};