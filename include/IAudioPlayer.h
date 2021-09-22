#pragma once

#include <AL/al.h>
#include <memory>
#include <string>

enum class AudioMode
{
	Normal = 0,
	Loop
};
enum class AudioState
{
	Playing = 0,
	Paused,
	Stopped
};
struct AudioProps
{
	AudioMode mode;
	
	// Default sizes for streaming mode
	// if uncompressed data is larger than the following,
	// stream mode will be enabled
	//
	size_t streamBufferCount = 4;
	size_t streamBufferSize = 64 * 1024; // 64kb
};

class IAudioPlayer
{
public:
	IAudioPlayer(const AudioProps &props) : m_Props(props) {}
	virtual ~IAudioPlayer() = default;

	virtual void Play() = 0;
	virtual void Stop() = 0;
	virtual void Pause() = 0;

	virtual void SetGain(float gain) { alSourcef(m_SourceID, AL_GAIN, gain); }
	virtual void SetPosition(float x, float y, float z) { alSource3f(m_SourceID, AL_POSITION, x, y, z); }
	virtual void SetPitch(float pitch) { alSourcef(m_SourceID, AL_PITCH, pitch); }

	virtual AudioState PollState() const;
	virtual bool IsPlaying() const { return PollState() == AudioState::Playing; }
	virtual bool IsStopped() const { return PollState() == AudioState::Stopped; }

protected:
	AudioProps m_Props;
	ALuint m_SourceID = 0;

public:
	static std::shared_ptr<IAudioPlayer> LoadFromFile(
		const std::string& path, 
		const AudioProps& props = { AudioMode::Normal });
};