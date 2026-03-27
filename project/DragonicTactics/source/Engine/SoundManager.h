/*
Must Read!!!!!!!!!!!!!!!!!!!

OGG - BGM
Wav - SFX
*/


#pragma once

#include <al.h>
#include <alc.h>
#include <string>
#include <map>

class SoundManager
{
public:
    SoundManager()  = default;
    ~SoundManager() = default;

    void Init();
    void Shutdown();
    void LoadBGM(const std::string& ogg_path);
    void PlayBGM(const std::string& ogg_path, bool loop = true);
    void PauseBGM();
    void ResumeBGM();
    void StopBGM();
    void SetBGMLoop(bool loop);
    void SetBGMVolume(float volume);
    void LoadSFX(const std::string& wav_path);
    void PlaySFX(const std::string& wav_path);
    void StopAllSFX();
    void SetSFXVolume(float volume);

    float GetBGMVolume() const;
    float GetSFXVolume() const;
    bool  IsBGMPlaying() const;
    bool  IsBGMPaused() const;
private:
    ALCdevice*  al_device_  = nullptr;
    ALCcontext* al_context_ = nullptr;

    ALuint bgm_source_ = 0;
    bool   bgm_loaded_ = false;

    std::map<std::string, ALuint> bgm_cache_;
    std::map<std::string, ALuint> sfx_cache_;

    static constexpr int kSfxSourcePoolSize = 8;
    ALuint               sfx_sources_[kSfxSourcePoolSize]{};

    float bgm_volume_ = 1.0f;
    float sfx_volume_ = 1.0f;

    bool   LoadOGGToBuffer(const std::string& path, ALuint& out_buffer);
    bool   LoadWAVToBuffer(const std::string& path, ALuint& out_buffer);
    ALuint GetFreeSFXSource();
};
