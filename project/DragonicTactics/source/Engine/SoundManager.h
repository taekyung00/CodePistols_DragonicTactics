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
#include <vector>

class SoundManager
{
public:
    static constexpr const char* BGM_MAIN_MENU = "Assets/Audio/BGM/BGM_Main.ogg";
    static constexpr const char* BGM_BATTLE    = "Assets/Audio/BGM/BGM_test.ogg";
    static constexpr const char* BGM_WIN       = "Assets/Audio/BGM/BGM_win.ogg";
    static constexpr const char* BGM_LOSE      = "Assets/Audio/BGM/BGM_lose.ogg";
    static constexpr const char* SFX_HIT       = "Assets/Audio/SFX/SFX_test.wav";

    static constexpr const char* SFX_DRAGON_ACTION  = "Assets/Audio/SFX/dragon_action.wav";
    static constexpr const char* SFX_DRAGON_HURT    = "Assets/Audio/SFX/dragon_hurt.wav";
    static constexpr const char* SFX_DRAGON_WALK    = "Assets/Audio/SFX/dragon_walk.wav";
    static constexpr const char* SFX_FIGHTER_ACTION = "Assets/Audio/SFX/fighter_action.wav";
    static constexpr const char* SFX_FIGHTER_HURT   = "Assets/Audio/SFX/fighter_hurt.wav";
    static constexpr const char* SFX_CLERIC_ACTION  = "Assets/Audio/SFX/cleric_action.wav";
    static constexpr const char* SFX_CLERIC_HURT    = "Assets/Audio/SFX/cleric_hurt.wav";
    static constexpr const char* SFX_ROGUE_ACTION   = "Assets/Audio/SFX/rouge_action.wav";
    static constexpr const char* SFX_ROGUE_HURT     = "Assets/Audio/SFX/rouge_hurt.wav";
    static constexpr const char* SFX_WIZARD_ACTION  = "Assets/Audio/SFX/wizard_action.wav";
    static constexpr const char* SFX_WIZARD_HURT    = "Assets/Audio/SFX/wizard_hurt.wav";
    static constexpr const char* SFX_HUMAN_WALK     = "Assets/Audio/SFX/human_walk.wav";
    static constexpr const char* SFX_BUTTON_CLICK   = "Assets/Audio/SFX/ButtonClick.wav";

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
    void PlaySFX(const std::string& wav_path);           // 앞에서 round-robin — 공격/스펠 SFX용
    void PlaySFXLast(const std::string& wav_path);       // 끝에서 역방향 탐색 — hurt SFX 전용
    void PlaySFXDelayed(const std::string& wav_path, double delay_seconds);
    void ClearPendingDelayedSFX() { pending_sfx_.clear(); }
    void StopAllSFX();
    void SetSFXVolume(float volume);
    void Update(double dt);

    // Debug 훅: PlaySFX 호출 직후 wav_path를 받아 호출됨. 한 개 콜백만 보관(디버그 용도).
    using SfxCallback = std::function<void(const std::string&)>;
    void SetSfxCallback(SfxCallback cb) { sfx_callback_ = std::move(cb); }
    void ClearSfxCallback() { sfx_callback_ = nullptr; }

    float  GetBGMVolume() const;
    float  GetSFXVolume() const;
    bool   IsBGMPlaying() const;
    bool   IsBGMPaused() const;
    double GetSFXDuration(const std::string& wav_path) const;  // 로드된 WAV의 재생 시간(초)
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

    // 지연 재생 SFX 큐
    struct DelayedSFX
    {
        std::string path;
        double      timer;
    };
    std::vector<DelayedSFX> pending_sfx_;

    // round-robin 소스 할당 — 연속 PlaySFX 호출 시 같은 소스 재사용 방지
    int last_sfx_source_index_ = -1;

    bool   LoadOGGToBuffer(const std::string& path, ALuint& out_buffer);
    bool   LoadWAVToBuffer(const std::string& path, ALuint& out_buffer);
    ALuint GetFreeSFXSource();     // 앞에서 round-robin
    ALuint GetLastFreeSFXSource(); // 끝에서 역방향 탐색
    
};
