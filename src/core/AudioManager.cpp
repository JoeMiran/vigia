#include "core/AudioManager.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <iostream>
#include <fstream>

AudioManager::AudioManager() {}

AudioManager::~AudioManager() {
    shutdown();
}

bool AudioManager::init() {
    if (m_initialized) return true;

    m_engine = new ma_engine();
    ma_result result = ma_engine_init(NULL, m_engine);
    if (result != MA_SUCCESS) {
        std::cerr << "[Audio] Falha ao inicializar engine: " << result << "\n";
        delete m_engine;
        m_engine = nullptr;
        return false;
    }

    m_music = new ma_sound();
    m_initialized = true;
    std::cout << "[Audio] Inicializado com sucesso\n";
    return true;
}

void AudioManager::shutdown() {
    if (!m_initialized) return;

    stopMusic();

    if (m_music) {
        delete m_music;
        m_music = nullptr;
    }
    if (m_engine) {
        ma_engine_uninit(m_engine);
        delete m_engine;
        m_engine = nullptr;
    }
    m_initialized = false;
    std::cout << "[Audio] Finalizado\n";
}

bool AudioManager::playMusic(const std::string& path, bool loop) {
    if (!m_initialized || !m_engine) return false;

    stopMusic();

    ma_uint32 flags = loop ? MA_SOUND_FLAG_LOOPING : 0;
    ma_result result = ma_sound_init_from_file(m_engine, path.c_str(), flags, NULL, NULL, m_music);
    if (result != MA_SUCCESS) {
        std::cerr << "[Audio] Falha ao carregar arquivo: " << path << " (erro " << result << ")\n";
        return false;
    }

    ma_sound_start(m_music);
    std::cout << "[Audio] Tocando: " << path << "\n";
    return true;
}

void AudioManager::stopMusic() {
    if (!m_music) return;
    if (ma_sound_is_playing(m_music)) {
        ma_sound_stop(m_music);
    }
    ma_sound_uninit(m_music);
}

void AudioManager::setMusicVolume(float volume) {
    if (!m_music) return;
    ma_sound_set_volume(m_music, volume);
}

bool AudioManager::isMusicPlaying() const {
    if (!m_music) return false;
    return ma_sound_is_playing(m_music);
}
