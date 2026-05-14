/*
Must Read!!!!!!!!!!!!!!!!!!!

OGG - BGM
Wav - SFX
*/


#pragma once

#include <al.h>
#include <alc.h>
#include <functional>
#include <string>
#include <map>

class SoundManager
{
public:
    static constexpr const char* BGM_MAIN_MENU = "Assets/Audio/BGM/BGM_Main.ogg";
    static constexpr const char* BGM_BATTLE    = "Assets/Audio/BGM/BGM_test.ogg";
    static constexpr const char* SFX_HIT       = "Assets/Audio/SFX/SFX_test.wav";

    static constexpr const char* SFX_DRAGON_ACTION  = "Assets/Audio/SFX/dragon_action.wav";
    static constexpr const char* SFX_DRAGON_HURT    = "Assets/Audio/SFX/dragon_hurt.wav";
    static constexpr const char* SFX_DRAGON_WALK    = "Assets/Audio/SFX/dragon_walk.wav";
    static constexpr const char* SFX_FIGHTER_ACTION = "Assets/Audio/SFX/fighter_action.wav";
    static constexpr const char* SFX_FIGHTER_HURT   = "Assets/Audio/SFX/fighter_hurt.wav";
    static constexpr const char* SFX_CLERIC_ACTION  = "Assets/Audio/SFX/cleric_action.wav";
    static constexpr const char* SFX_CLERIC_HURT    = "Assets/Audio/SFX/cleric_hurt.wav";
    static constexpr const char* SFX_HUMAN_WALK     = "Assets/Audio/SFX/human_walk.wav";

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

    // Debug 훅: PlaySFX 호출 직후 wav_path를 받아 호출됨. 한 개 콜백만 보관(디버그 용도).
    using SfxCallback = std::function<void(const std::string&)>;
    void SetSfxCallback(SfxCallback cb) { sfx_callback_ = std::move(cb); }
    void ClearSfxCallback() { sfx_callback_ = nullptr; }

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

    SfxCallback sfx_callback_;

    bool   LoadOGGToBuffer(const std::string& path, ALuint& out_buffer);
    bool   LoadWAVToBuffer(const std::string& path, ALuint& out_buffer);
    ALuint GetFreeSFXSource();
    
};
