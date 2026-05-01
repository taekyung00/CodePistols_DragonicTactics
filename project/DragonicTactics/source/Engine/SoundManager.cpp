#include "pch.h"
#include "SoundManager.h"

#include <SDL.h>
#include "Path.h"

#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>

void SoundManager::Init()
{
    al_device_ = alcOpenDevice(nullptr);
    if (!al_device_)
    {
        Engine::GetLogger().LogError("SoundManager: alcOpenDevice failed");
        return;
    }

    al_context_ = alcCreateContext(al_device_, nullptr);
    if (!al_context_ || !alcMakeContextCurrent(al_context_))
    {
        Engine::GetLogger().LogError("SoundManager: alcCreateContext / MakeCurrent failed");
        return;
    }

    alGenSources(1, &bgm_source_);

    alGenSources(kSfxSourcePoolSize, sfx_sources_);

    Engine::GetLogger().LogEvent("SoundManager: Initialized");
}


void SoundManager::Shutdown()
{
    StopBGM();
    StopAllSFX();

    for (auto& [path, buffer] : bgm_cache_)
        alDeleteBuffers(1, &buffer);
    bgm_cache_.clear();

    for (auto& [path, buffer] : sfx_cache_)
        alDeleteBuffers(1, &buffer);
    sfx_cache_.clear();

    alDeleteSources(1, &bgm_source_);
    alDeleteSources(kSfxSourcePoolSize, sfx_sources_);

    alcMakeContextCurrent(nullptr);
    if (al_context_)
    {
        alcDestroyContext(al_context_);
        al_context_ = nullptr;
    }
    if (al_device_)
    {
        alcCloseDevice(al_device_);
        al_device_ = nullptr;
    }

    Engine::GetLogger().LogEvent("SoundManager: Shutdown");
}

void SoundManager::LoadBGM(const std::string& ogg_path)
{
    if(bgm_cache_.count(ogg_path)){
        return;
    }

    ALuint buffer = 0;
    if(!LoadOGGToBuffer(ogg_path, buffer))
    {
        Engine::GetLogger().LogError("Failed to load BGM" + ogg_path);
        return;
    }
    bgm_cache_[ogg_path] = buffer;
}

void SoundManager::PlayBGM(const std::string& ogg_path, bool loop)
{
    auto it = bgm_cache_.find(ogg_path);
    if(it == bgm_cache_.end()){
        return;
    }

    StopBGM();

    alSourcei(bgm_source_, AL_BUFFER, static_cast<ALint>(it->second));
    alSourcei(bgm_source_, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    alSourcef(bgm_source_, AL_GAIN, bgm_volume_);
    alSourcePlay(bgm_source_);
    bgm_loaded_ = true;

    Engine::GetLogger().LogEvent("Playing Bgm - " + ogg_path);
}

void SoundManager::PauseBGM()
{
    if (bgm_loaded_)
        alSourcePause(bgm_source_);
}

void SoundManager::ResumeBGM()
{
    if (bgm_loaded_)
    {
        ALint state = 0;
        alGetSourcei(bgm_source_, AL_SOURCE_STATE, &state);
        if (state == AL_PAUSED)
            alSourcePlay(bgm_source_);
    }
}

void SoundManager::StopBGM()
{
    if (bgm_loaded_)
    {
        alSourceStop(bgm_source_);
        alSourcei(bgm_source_, AL_BUFFER, 0);

        bgm_loaded_ = false;
    }
}

void SoundManager::SetBGMLoop(bool loop)
{
    alSourcei(bgm_source_, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

void SoundManager::SetBGMVolume(float volume)
{
    bgm_volume_ = (volume < 0.0f) ? 0.0f : (volume > 1.0f) ? 1.0f : volume;
    alSourcef(bgm_source_, AL_GAIN, bgm_volume_);
}

void SoundManager::LoadSFX(const std::string& wav_path)
{
    if (sfx_cache_.count(wav_path))
        return;

    ALuint buffer = 0;
    if (!LoadWAVToBuffer(wav_path, buffer))
    {
        Engine::GetLogger().LogError("SoundManager: Failed to load SFX – " + wav_path);
        return;
    }
    sfx_cache_[wav_path] = buffer;
}

void SoundManager::PlaySFX(const std::string& wav_path)
{
    auto it = sfx_cache_.find(wav_path);
    if (it == sfx_cache_.end())
        return;

    ALuint source = GetFreeSFXSource();
    if (source == 0)
    {
        Engine::GetLogger().LogError("SoundManager: No free SFX source available");
        return;
    }

    alSourcei(source, AL_BUFFER, static_cast<ALint>(it->second));
    alSourcei(source, AL_LOOPING, AL_FALSE);
    alSourcef(source, AL_GAIN, sfx_volume_);
    alSourcePlay(source);

    if (sfx_callback_)
        sfx_callback_(wav_path);
}

void SoundManager::StopAllSFX()
{
    for (int i = 0; i < kSfxSourcePoolSize; ++i)
        alSourceStop(sfx_sources_[i]);
}

void SoundManager::SetSFXVolume(float volume)
{
    sfx_volume_ = (volume < 0.0f) ? 0.0f : (volume > 1.0f) ? 1.0f : volume;
}

bool SoundManager::LoadOGGToBuffer(const std::string& path, ALuint& out_buffer)
{   
    int    channels    = 0;
    int    sample_rate = 0;
    short* output      = nullptr;

    std::string resolved = assets::locate_asset(path).string();
    Engine::GetLogger().LogEvent("SoundManager: resolved path = " + resolved);

    int samples = stb_vorbis_decode_filename(resolved.c_str(), &channels, &sample_rate, &output);
    if (samples == -1 || output == nullptr)
    {
        Engine::GetLogger().LogError("SoundManager: stb_vorbis failed – resolved: " + resolved);
        return false;
    }

    ALenum  format = (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    ALsizei size   = samples * channels * static_cast<ALsizei>(sizeof(short));

    alGenBuffers(1, &out_buffer);
    alBufferData(out_buffer, format, output, size, static_cast<ALsizei>(sample_rate));

    free(output);
    return true;
}

bool SoundManager::LoadWAVToBuffer(const std::string& path, ALuint& out_buffer)
{
    SDL_AudioSpec wav_spec{};
    Uint32        wav_length = 0;
    Uint8*        wav_buffer = nullptr;

    std::string resolved = assets::locate_asset(path).string();

    if (!SDL_LoadWAV(resolved.c_str(), &wav_spec, &wav_buffer, &wav_length))
    {
        Engine::GetLogger().LogError(std::string("SoundManager: SDL_LoadWAV failed – ") + SDL_GetError());
        return false;
    }

    ALenum format = AL_NONE;
    if (wav_spec.channels == 1)
    {
        if (wav_spec.format == AUDIO_U8)
            format = AL_FORMAT_MONO8;
        else if (wav_spec.format == AUDIO_S16SYS)
            format = AL_FORMAT_MONO16;
    }
    else if (wav_spec.channels == 2)
    {
        if (wav_spec.format == AUDIO_U8)
            format = AL_FORMAT_STEREO8;
        else if (wav_spec.format == AUDIO_S16SYS)
            format = AL_FORMAT_STEREO16;
    }

    if (format == AL_NONE)
    {
        Engine::GetLogger().LogError("SoundManager: Unsupported WAV format – " + path);
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
    for (int i = 0; i < kSfxSourcePoolSize; ++i)
    {
        ALint state = 0;
        alGetSourcei(sfx_sources_[i], AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING)
            return sfx_sources_[i];
    }
    return 0;
}

float SoundManager::GetBGMVolume() const 
{ 
    return bgm_volume_;
}
float SoundManager::GetSFXVolume() const 
{
    return sfx_volume_;
}

bool SoundManager::IsBGMPlaying() const
{
    if (!bgm_loaded_) return false;
    ALint state = 0;
    alGetSourcei(bgm_source_, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}

bool SoundManager::IsBGMPaused() const
{
    if (!bgm_loaded_) return false;
    ALint state = 0;
    alGetSourcei(bgm_source_, AL_SOURCE_STATE, &state);
    return state == AL_PAUSED;
}
