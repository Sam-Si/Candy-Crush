#include "AudioController.h"
#include <iostream>

AudioController& AudioController::getInstance()
{
    static AudioController instance;
    return instance;
}

AudioController::AudioController() {}
AudioController::~AudioController() { cleanup(); }

bool AudioController::init()
{
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    // Load default sounds
    loadSound("match", "assets/audio/match.wav");
    loadSound("swap", "assets/audio/swap.wav");
    loadSound("gameover", "assets/audio/gameover.wav");

    return true;
}

void AudioController::cleanup()
{
    if (bgm != nullptr)
    {
        Mix_FreeMusic(bgm);
        bgm = nullptr;
    }

    for (auto const& [name, chunk] : sounds)
    {
        if (chunk != nullptr)
        {
            Mix_FreeChunk(chunk);
        }
    }
    sounds.clear();

    Mix_Quit();
}

void AudioController::loadSound(const std::string& name, const std::string& path)
{
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (chunk == nullptr)
    {
        std::cerr << "Failed to load sound " << path << "! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }
    else
    {
        sounds[name] = chunk;
    }
}

void AudioController::playSound(const std::string& name)
{
    auto it = sounds.find(name);
    if (it != sounds.end() && it->second != nullptr)
    {
        Mix_PlayChannel(-1, it->second, 0);
    }
}

void AudioController::playBGM(const std::string& path)
{
    stopBGM();
    bgm = Mix_LoadMUS(path.c_str());
    if (bgm == nullptr)
    {
        std::cerr << "Failed to load BGM " << path << "! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }
    else
    {
        Mix_PlayMusic(bgm, -1);
    }
}

void AudioController::stopBGM()
{
    if (bgm != nullptr)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
        bgm = nullptr;
    }
}
