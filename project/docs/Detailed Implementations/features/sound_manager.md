# 사운드 매니저 구현 계획

**프로젝트**: Dragonic Tactics - 턴제 전술 RPG
**기능**: SoundManager - SDL2 + OpenAL 기반 사운드 싱글톤 서비스
**작성일**: 2026-03-08

**참고 자료**:

- [docs/hw1-source-1.txt](../../hw1-source-1.txt) - SDL2+OpenAL 사운드 참고 구현
- [Engine.cpp](../../../DragonicTactics/source/Engine/Engine.cpp) - 싱글톤 서비스 패턴 참고
- [TextManager.h](../../../DragonicTactics/source/Engine/TextManager.h) - 싱글톤 서비스 구조 참고

---

## 개요

hw1의 구현을 바탕으로:

- **BGM**: OGG 파일 → `stb_vorbis`로 디코딩 → OpenAL로 재생 (루프 가능)
- **SFX**: WAV 파일 → `SDL_LoadWAV`로 로드 → OpenAL로 재생 (단발성)

엔진의 다른 싱글톤 서비스(TextManager, TextureManager 등)와 동일하게 `Engine::GetSoundManager()`로 접근.

---

## Implementation Tasks

### Task 1: SoundManager 헤더 설계

**파일 생성**: `DragonicTactics/source/Engine/SoundManager.h`

```cpp
/**
 * \file SoundManager.h
 * \brief SDL2 + OpenAL 기반 사운드 매니저
 *        BGM: OGG (루프), SFX: WAV (단발)
 */
#pragma once
#include <al.h>
#include <alc.h>
#include <string>
#include <map>

class SoundManager
{
public:
    SoundManager() = default;
    ~SoundManager() = default;

    // Engine::Start()에서 호출
    void Init();

    // Engine::Stop()에서 호출
    void Shutdown();

    // ============================================================
    // BGM (OGG) - 배경음악, 루프 재생
    // ============================================================

    /// @brief BGM 로드 및 재생
    /// @param ogg_path Assets/ 기준 경로 (예: "Assets/Audio/battle_bgm.ogg")
    /// @param loop     루프 여부 (기본 true)
    void PlayBGM(const std::string& ogg_path, bool loop = true);

    /// @brief BGM 일시정지
    void PauseBGM();

    /// @brief BGM 재개
    void ResumeBGM();

    /// @brief BGM 정지 및 소스 해제
    void StopBGM();

    /// @brief BGM 루프 온/오프 전환
    void SetBGMLoop(bool loop);

    /// @brief BGM 볼륨 설정 (0.0 ~ 1.0)
    void SetBGMVolume(float volume);

    // ============================================================
    // SFX (WAV) - 효과음, 단발 재생
    // ============================================================

    /// @brief SFX 로드 (처음 한 번만 로드, 이후 캐시 사용)
    /// @param wav_path Assets/ 기준 경로 (예: "Assets/Audio/sword_hit.wav")
    void LoadSFX(const std::string& wav_path);

    /// @brief SFX 재생 (자동 로드 포함)
    void PlaySFX(const std::string& wav_path);

    /// @brief 재생 중인 모든 SFX 정지
    void StopAllSFX();

    /// @brief SFX 볼륨 설정 (0.0 ~ 1.0)
    void SetSFXVolume(float volume);

private:
    // OpenAL 컨텍스트
    ALCdevice*  al_device_  = nullptr;
    ALCcontext* al_context_ = nullptr;

    // BGM 상태
    ALuint bgm_buffer_ = 0;
    ALuint bgm_source_ = 0;
    bool   bgm_loaded_ = false;

    // SFX 캐시: 경로 → OpenAL 버퍼 핸들
    std::map<std::string, ALuint> sfx_cache_;

    // SFX 소스 풀 (동시 재생을 위해 복수 소스)
    static constexpr int SFX_SOURCE_POOL_SIZE = 8;
    ALuint sfx_sources_[SFX_SOURCE_POOL_SIZE] = {};

    // 볼륨
    float bgm_volume_ = 1.0f;
    float sfx_volume_ = 1.0f;

    // 헬퍼
    bool        LoadOGGToBuffer(const std::string& path, ALuint& out_buffer);
    bool        LoadWAVToBuffer(const std::string& path, ALuint& out_buffer);
    ALuint      GetFreeSFXSource();   // 사용 가능한 소스 찾기
    ALenum      GetOpenALFormat(int channels, SDL_AudioFormat format) const;
};
```

---

### Task 2: SoundManager 구현

**파일 생성**: `DragonicTactics/source/Engine/SoundManager.cpp`

```cpp
#include "pch.h"
#include "SoundManager.h"
#include "Engine.h"
#include <SDL.h>

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"  // OGG 디코딩

void SoundManager::Init()
{
    al_device_  = alcOpenDevice(nullptr);
    al_context_ = alcCreateContext(al_device_, nullptr);
    alcMakeContextCurrent(al_context_);

    // BGM 소스 생성
    alGenSources(1, &bgm_source_);

    // SFX 소스 풀 생성
    alGenSources(SFX_SOURCE_POOL_SIZE, sfx_sources_);

    Engine::GetLogger().LogEvent("SoundManager: Initialized");
}

void SoundManager::Shutdown()
{
    StopBGM();
    StopAllSFX();

    // 버퍼 해제
    if (bgm_buffer_) alDeleteBuffers(1, &bgm_buffer_);
    for (auto& [path, buffer] : sfx_cache_)
        alDeleteBuffers(1, &buffer);

    alDeleteSources(1, &bgm_source_);
    alDeleteSources(SFX_SOURCE_POOL_SIZE, sfx_sources_);

    alcMakeContextCurrent(nullptr);
    alcDestroyContext(al_context_);
    alcCloseDevice(al_device_);

    al_device_  = nullptr;
    al_context_ = nullptr;

    Engine::GetLogger().LogEvent("SoundManager: Shutdown");
}

// ============================================================
// BGM
// ============================================================

void SoundManager::PlayBGM(const std::string& ogg_path, bool loop)
{
    StopBGM(); // 기존 BGM 정지

    if (!LoadOGGToBuffer(ogg_path, bgm_buffer_))
    {
        Engine::GetLogger().LogError("SoundManager: Failed to load BGM: " + ogg_path);
        return;
    }

    alSourcei(bgm_source_, AL_BUFFER, static_cast<ALint>(bgm_buffer_));
    alSourcei(bgm_source_, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    alSourcef(bgm_source_, AL_GAIN, bgm_volume_);
    alSourcePlay(bgm_source_);
    bgm_loaded_ = true;

    Engine::GetLogger().LogEvent("SoundManager: BGM playing - " + ogg_path);
}

void SoundManager::PauseBGM()
{
    if (bgm_loaded_)
        alSourcePause(bgm_source_);
}

void SoundManager::ResumeBGM()
{
    if (bgm_loaded_)
        alSourcePlay(bgm_source_);
}

void SoundManager::StopBGM()
{
    if (bgm_loaded_)
    {
        alSourceStop(bgm_source_);
        alSourcei(bgm_source_, AL_BUFFER, 0);
        if (bgm_buffer_)
        {
            alDeleteBuffers(1, &bgm_buffer_);
            bgm_buffer_ = 0;
        }
        bgm_loaded_ = false;
    }
}

void SoundManager::SetBGMLoop(bool loop)
{
    alSourcei(bgm_source_, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

void SoundManager::SetBGMVolume(float volume)
{
    bgm_volume_ = volume;
    alSourcef(bgm_source_, AL_GAIN, bgm_volume_);
}

// ============================================================
// SFX
// ============================================================

void SoundManager::LoadSFX(const std::string& wav_path)
{
    if (sfx_cache_.count(wav_path)) return; // 이미 로드됨

    ALuint buffer = 0;
    if (!LoadWAVToBuffer(wav_path, buffer))
    {
        Engine::GetLogger().LogError("SoundManager: Failed to load SFX: " + wav_path);
        return;
    }
    sfx_cache_[wav_path] = buffer;
}

void SoundManager::PlaySFX(const std::string& wav_path)
{
    LoadSFX(wav_path); // 미로드 시 자동 로드

    auto it = sfx_cache_.find(wav_path);
    if (it == sfx_cache_.end()) return;

    ALuint source = GetFreeSFXSource();
    if (source == 0)
    {
        Engine::GetLogger().LogError("SoundManager: No free SFX source");
        return;
    }

    alSourcei(source, AL_BUFFER, static_cast<ALint>(it->second));
    alSourcei(source, AL_LOOPING, AL_FALSE);
    alSourcef(source, AL_GAIN, sfx_volume_);
    alSourcePlay(source);
}

void SoundManager::StopAllSFX()
{
    for (int i = 0; i < SFX_SOURCE_POOL_SIZE; ++i)
        alSourceStop(sfx_sources_[i]);
}

void SoundManager::SetSFXVolume(float volume)
{
    sfx_volume_ = volume;
}

// ============================================================
// 헬퍼 구현
// ============================================================

bool SoundManager::LoadOGGToBuffer(const std::string& path, ALuint& out_buffer)
{
    int    channels, sample_rate;
    short* output  = nullptr;
    int    samples = stb_vorbis_decode_filename(path.c_str(), &channels, &sample_rate, &output);
    if (samples == -1 || output == nullptr) return false;

    ALenum format = (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    ALsizei size  = samples * channels * static_cast<int>(sizeof(short));

    alGenBuffers(1, &out_buffer);
    alBufferData(out_buffer, format, output, size, sample_rate);
    free(output);
    return true;
}

bool SoundManager::LoadWAVToBuffer(const std::string& path, ALuint& out_buffer)
{
    SDL_AudioSpec wav_spec{};
    Uint32        wav_length = 0;
    Uint8*        wav_buffer = nullptr;

    if (!SDL_LoadWAV(path.c_str(), &wav_spec, &wav_buffer, &wav_length))
    {
        Engine::GetLogger().LogError(std::string("SDL_LoadWAV failed: ") + SDL_GetError());
        return false;
    }

    ALenum format = GetOpenALFormat(wav_spec.channels, wav_spec.format);
    if (format == AL_NONE)
    {
        SDL_FreeWAV(wav_buffer);
        return false;
    }

    alGenBuffers(1, &out_buffer);
    alBufferData(out_buffer, format, wav_buffer,
                 static_cast<ALsizei>(wav_length),
                 static_cast<ALsizei>(wav_spec.freq));
    SDL_FreeWAV(wav_buffer);
    return true;
}

ALuint SoundManager::GetFreeSFXSource()
{
    for (int i = 0; i < SFX_SOURCE_POOL_SIZE; ++i)
    {
        ALint state;
        alGetSourcei(sfx_sources_[i], AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING)
            return sfx_sources_[i];
    }
    return 0; // 모두 사용 중
}

ALenum SoundManager::GetOpenALFormat(int channels, SDL_AudioFormat format) const
{
    if (channels == 1 && format == AUDIO_U8)    return AL_FORMAT_MONO8;
    if (channels == 1 && format == AUDIO_S16SYS) return AL_FORMAT_MONO16;
    if (channels == 2 && format == AUDIO_U8)    return AL_FORMAT_STEREO8;
    if (channels == 2 && format == AUDIO_S16SYS) return AL_FORMAT_STEREO16;
    return AL_NONE;
}
```

---

### Task 3: Engine에 SoundManager 통합

**파일**: `Engine.h` - 멤버 선언 추가:

```cpp
// Engine.h
#include "SoundManager.h"

class Engine
{
public:
    static SoundManager& GetSoundManager();
    // ... 기존 선언
};
```

**파일**: `Engine.cpp` - Impl에 추가:

```cpp
// Engine.cpp - Impl 클래스
class Engine::Impl
{
public:
    // ... 기존 멤버
    SoundManager soundManager{};
};

SoundManager& Engine::GetSoundManager()
{
    return Instance().impl->soundManager;
}

void Engine::Start(std::string_view window_title)
{
    // ... 기존 코드
    impl->soundManager.Init();  // TextManager::Init() 근처에 추가
}

void Engine::Stop()
{
    impl->soundManager.Shutdown();  // TextureManager::Shutdown() 근처에 추가
    // ... 기존 코드
}
```

---

### Task 4: 게임에서 SoundManager 사용

**GamePlay::Load()에서 BGM 시작**:

```cpp
void GamePlay::Load()
{
    // ... 기존 코드
    Engine::GetSoundManager().PlayBGM("Assets/Audio/battle_bgm.ogg", true);
}

void GamePlay::Unload()
{
    Engine::GetSoundManager().StopBGM();
    // ... 기존 코드
}
```

**전투 이벤트에서 SFX**:

```cpp
// EventBus 구독 (GamePlay::Load())
GetGSComponent<EventBus>()->Subscribe<CharacterDamagedEvent>(
    [](const CharacterDamagedEvent& event) {
        Engine::GetSoundManager().PlaySFX("Assets/Audio/sword_hit.wav");
    });

GetGSComponent<EventBus>()->Subscribe<CharacterDeathEvent>(
    [](const CharacterDeathEvent& event) {
        Engine::GetSoundManager().PlaySFX("Assets/Audio/character_death.wav");
    });
```

---

## Implementation Examples

### 기본 사용 패턴

```cpp
// BGM 재생 (루프)
Engine::GetSoundManager().PlayBGM("Assets/Audio/battle_bgm.ogg");

// BGM 일시정지 (게임 일시정지 시)
Engine::GetSoundManager().PauseBGM();

// BGM 재개
Engine::GetSoundManager().ResumeBGM();

// SFX 재생 (공격 시)
Engine::GetSoundManager().PlaySFX("Assets/Audio/sword_swing.wav");

// 볼륨 조절
Engine::GetSoundManager().SetBGMVolume(0.5f);  // BGM 50%
Engine::GetSoundManager().SetSFXVolume(0.8f);  // SFX 80%

// 루프 토글
Engine::GetSoundManager().SetBGMLoop(false);  // 한 번만 재생
```

### 디렉토리 구조 권장

```
DragonicTactics/Assets/Audio/
├── BGM/
│   ├── battle_bgm.ogg
│   └── main_menu_bgm.ogg
└── SFX/
    ├── sword_hit.wav
    ├── spell_cast.wav
    ├── character_death.wav
    └── turn_start.wav
```

---

## Rigorous Testing

### 테스트 1: 초기화 및 종료

```cpp
// Engine::Start() 후 SoundManager가 정상 초기화됨
SoundManager& sm = Engine::GetSoundManager();
// OGG 로드 및 재생
sm.PlayBGM("Assets/Audio/test.ogg");
// 크래시 없이 실행되어야 함
sm.StopBGM();
```

### 테스트 2: SFX 동시 재생

```cpp
// 8개의 SFX 소스 풀로 동시 재생 가능
for (int i = 0; i < 8; ++i)
    Engine::GetSoundManager().PlaySFX("Assets/Audio/sword_hit.wav");
// 9번째는 소스가 없어도 크래시 없이 LogError만 발생
Engine::GetSoundManager().PlaySFX("Assets/Audio/sword_hit.wav");
```

### 테스트 3: BGM 루프 토글

```cpp
sm.PlayBGM("Assets/Audio/battle_bgm.ogg", true);  // 루프 재생
// 3초 후
sm.SetBGMLoop(false);  // 루프 중지 (현재 재생 끝나면 정지)
```

### 테스트 4: 볼륨 범위

```cpp
// 0.0 ~ 1.0 범위 테스트
sm.SetBGMVolume(0.0f);   // 무음
sm.SetBGMVolume(1.0f);   // 최대
sm.SetBGMVolume(0.5f);   // 중간
// 범위 밖 값은 클램핑 처리 (추가 안전장치)
```

### 게임 내 수동 테스트 키

```cpp
// GamePlay.cpp에 추가
if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::M))
    Engine::GetSoundManager().PauseBGM();   // M: BGM 일시정지/재개 토글
```
