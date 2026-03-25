#pragma once

#include <SDL2/SDL_mixer.h>
#include <string>
#include <map>

class AudioController
{
public:
    static AudioController& getInstance();

    bool init();
    void cleanup();

    void playBGM(const std::string& path);
    void stopBGM();

    void playSound(const std::string& name);
    void loadSound(const std::string& name, const std::string& path);

private:
    AudioController();
    ~AudioController();

    // Prevent copy and assignment
    AudioController(const AudioController&) = delete;
    AudioController& operator=(const AudioController&) = delete;

    Mix_Music* bgm = nullptr;
    std::map<std::string, Mix_Chunk*> sounds;
};
