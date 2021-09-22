
#include "AudioDevice.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <atomic>
#include <mutex>

#include <AL/alc.h>
#include <AL/al.h>

struct AudioDeviceData
{
    ALCdevice *device = nullptr;
    ALCcontext *context = nullptr;
    
    // Shared data:
    //  - audio->streams
    //  - audio->running
    //  - AudioStreamPlayer::streaming
    //
    std::thread streamer;
    std::atomic_bool running;
    std::mutex mutex;

    std::vector<AudioStreamPlayer*> streams;
};

static AudioDeviceData *audio = nullptr;

void AudioDevice::Init()
{
    audio = new AudioDeviceData;

    audio->device = alcOpenDevice(nullptr);
    if (!audio->device)
    {
        std::cerr << "ERROR: Could not load audio device" << std::endl;
        return;
    }

    audio->context = alcCreateContext(audio->device, nullptr);
    if (!audio->context)
    {
        std::cerr << "ERROR: Could not create audio context" << std::endl;
        return;
    }

    if (alcMakeContextCurrent(audio->context) != ALC_TRUE)
    {
        std::cerr << "ERROR: Could not make audio context current" << std::endl;
        return;
    }

    audio->running = true;
    audio->streamer = std::thread([]()
    {
        while (audio->running)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Harsh implementation of thread priority
            UpdateStreams();
        }
    });
}

void AudioDevice::Shutdown()
{
    audio->running = false;
    audio->streamer.join();

    alcMakeContextCurrent(audio->context);
    alcDestroyContext(audio->context);
    alcCloseDevice(audio->device);

    delete audio; audio = nullptr;
}

void AudioDevice::RegisterStream(AudioStreamPlayer *streamer)
{
    std::lock_guard lock{ audio->mutex };
    audio->streams.push_back(streamer);
}

// TODO: Not really the best to stall the main thread when unregistering/registering streams
//
// When called, method will lock until the stream can be removed
//      the stream should not be removed until it is certain that
//      that it has finished its last update.
//
void AudioDevice::UnregisterStream(AudioStreamPlayer *streamer)
{
    std::lock_guard lock{ audio->mutex };
    auto &streams = audio->streams;
    streams.erase(std::remove(streams.begin(), streams.end(), streamer), streams.end());
}

void AudioDevice::UpdateStreams()
{
    std::lock_guard lock{ audio->mutex }; // stream list will not be updated by other threads
                                          // while it is being updated.
    auto &streams = audio->streams;
    std::for_each(streams.begin(), streams.end(), [&](AudioStreamPlayer *stream)
    {
        stream->UpdateStream();
        if (stream->IsStopped())
            streams.erase(std::remove(streams.begin(), streams.end(), stream), streams.end());
    });
}
