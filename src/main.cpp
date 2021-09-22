
#include <iostream>

#include <AudioDevice.h>

#include <thread>

int main()
{
    AudioDevice::Init();

    {
        std::shared_ptr<IAudioPlayer> music = IAudioPlayer::LoadFromFile("res/bwv537.wav", { AudioMode::Loop });
        std::shared_ptr<IAudioPlayer> monster1 = IAudioPlayer::LoadFromFile("res/mnstr2-mono.wav");
        std::shared_ptr<IAudioPlayer> monster2 = IAudioPlayer::LoadFromFile("res/mnstr3-mono.wav");
        std::shared_ptr<IAudioPlayer> monster3 = IAudioPlayer::LoadFromFile("res/mnstr4-mono.wav");
        monster1->SetPosition(-1.0f, 0.1f,  0.0f);
        monster2->SetPosition( 0.0f, 0.1f,  1.0f);
        monster3->SetPosition( 1.0f, 0.1f, -1.0f);

        music->Play();

        std::this_thread::sleep_for(std::chrono::milliseconds(2500)); monster1->Play();
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); monster2->Play();
        std::this_thread::sleep_for(std::chrono::milliseconds(250)); monster3->Play();

        std::cout << "Enter to pause: "; std::cin.get(); music->Pause();
        std::cout << "Enter to resume: "; std::cin.get(); music->Play();
        std::cout << "Enter to terminate: "; std::cin.get();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    AudioDevice::Shutdown();
    return 0;
}