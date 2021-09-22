#include "IAudioFileLoader.h"

IAudioFileLoader::IAudioFileLoader(const std::string path)
    : m_FilePath(path)
{
    if (!drwav_init_file(&m_FileHandle, path.c_str(), nullptr))
        return; // Failed to open file

    m_Loaded = true;
    m_Channels = m_FileHandle.channels;
    m_SampleRate = m_FileHandle.sampleRate;
    m_TotalFrames = m_FileHandle.totalPCMFrameCount;
}

IAudioFileLoader::~IAudioFileLoader()
{
    m_Loaded = false;
    drwav_uninit(&m_FileHandle); // Close file handle
}

size_t IAudioFileLoader::ReadFrames(size_t framesToRead, unsigned char *data)
{
	return drwav_read_pcm_frames_s16(&m_FileHandle, framesToRead, (drwav_int16*) data);
}

void IAudioFileLoader::SeekToFrame(size_t frame)
{
	drwav_seek_to_pcm_frame(&m_FileHandle, frame);
}
