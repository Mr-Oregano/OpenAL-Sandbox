
#include "AudioStreamPlayer.h"
#include "AudioDevice.h"

#include <iostream>
#include <utility>

AudioStreamPlayer::AudioStreamPlayer(const AudioProps &props, std::unique_ptr<IAudioFileLoader> loader)
	: IAudioPlayer(props), m_Stream(
		props.streamBufferCount, 
		props.streamBufferSize, 
		std::forward<std::unique_ptr<IAudioFileLoader>>(loader))
{
	m_StreamCache = new unsigned char[m_Stream.bufferSize];
	AL_CALL(alGenSources(1, &m_SourceID));
	AL_CALL(alSourceQueueBuffers(m_SourceID, (ALsizei) m_Stream.bufferCount, m_Stream.buffers));
}

AudioStreamPlayer::~AudioStreamPlayer()
{
	Stop(); // Stop the audio if still playing
			// Method will stall until the stream has been unregistered
			// Only then is it safe to free resources

	ALint buffersProcessed = 0;
	AL_CALL(alGetSourcei(m_SourceID, AL_BUFFERS_PROCESSED, &buffersProcessed));
	AL_CALL(alSourceUnqueueBuffers(m_SourceID, (ALsizei) buffersProcessed, m_Stream.buffers));

	AL_CALL(alDeleteSources(1, &m_SourceID));
	delete[] m_StreamCache;
}

void AudioStreamPlayer::Play()
{
	switch (PollState())
	{
	case AudioState::Stopped:
		AudioDevice::RegisterStream(this); // Will wait until the stream can be registered
	case AudioState::Paused:
		m_Streaming = true;
		AL_CALL(alSourcei(m_SourceID, AL_LOOPING, AL_FALSE)); // Looping is handled by the stream
		AL_CALL(alSourcePlay(m_SourceID));
		break;
	}
}

void AudioStreamPlayer::Stop()
{
	switch (PollState())
	{
	case AudioState::Playing:
	case AudioState::Paused:
		m_Streaming = false;
		AL_CALL(alSourceStop(m_SourceID));
		AudioDevice::UnregisterStream(this); // Will wait until the stream can be unregistered
		ResetStream(); // It is safe to do this as stream is no longer being updated
	}
}

void AudioStreamPlayer::Pause()
{
	switch (PollState())
	{
	case AudioState::Playing:
		m_Streaming = false;
		AL_CALL(alSourcePause(m_SourceID));
	}
}

// Could potentially be running on a separate thread.
// The only shared data will be m_Streaming. Any other class members
// Should only get modified during UpdateStream()
//
void AudioStreamPlayer::UpdateStream()
{
	if (!m_Streaming)
		return;

	// If the queue was starved of buffers then
	// restart the audio and move forward by one buffer.
	if (IsStopped())
	{
		std::cout << "Skipping to next buffer..." << std::endl;
		AL_CALL(alSourcei(m_SourceID, AL_BYTE_OFFSET, (ALint) m_Stream.bufferSize));
		AL_CALL(alSourcePlay(m_SourceID));
	}
	//
	
	ALint buffersProcessed = 0;
	AL_CALL(alGetSourcei(m_SourceID, AL_BUFFERS_PROCESSED, &buffersProcessed));
	for (int i = 0; i < buffersProcessed; ++i)
	{
		IAudioFileLoader &loader = *m_Stream.loader;
		size_t framesToRead = m_Stream.bufferSize / loader.GetFrameSize();
		size_t framesRead = loader.ReadFrames(framesToRead, m_StreamCache);

		ALuint buffer = 0;
		AL_CALL(alSourceUnqueueBuffers(m_SourceID, 1, &buffer));
		AL_CALL(alBufferData(buffer, m_Stream.format, m_StreamCache, (ALsizei)(framesRead * loader.GetFrameSize()), (ALsizei) loader.GetSampleRate()));
		AL_CALL(alSourceQueueBuffers(m_SourceID, 1, &buffer));

		if (framesRead < framesToRead)
		{
			loader.SeekToFrame(0);
			if (m_Props.mode == AudioMode::Normal)
				m_Streaming = false;
		}
	}
}

// Sharing data with UpdateStream(). This method should ONLY be called when
// it is certain that UpdateStream() is NOT running.
//
void AudioStreamPlayer::ResetStream()
{
	m_Stream.loader->SeekToFrame(0);

	ALint buffersProcessed = 0;
	AL_CALL(alGetSourcei(m_SourceID, AL_BUFFERS_PROCESSED, &buffersProcessed));
	AL_CALL(alSourceUnqueueBuffers(m_SourceID, (ALsizei) buffersProcessed, m_Stream.buffers));

	m_Stream.LoadStreamData();
	
	AL_CALL(alSourceQueueBuffers(m_SourceID, (ALsizei)m_Stream.bufferCount, m_Stream.buffers));
}
