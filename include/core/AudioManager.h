#pragma once
#include <string>

struct ma_engine;
struct ma_sound;

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool init();
    void shutdown();

    bool playMusic(const std::string& path, bool loop = true);
    void stopMusic();
    void setMusicVolume(float volume);
    bool isMusicPlaying() const;

private:
    ma_engine* m_engine = nullptr;
    ma_sound* m_music = nullptr;
    bool m_initialized = false;
};
