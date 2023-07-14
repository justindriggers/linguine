#pragma once

#include <audio/AudioManager.h>

#include <AL/al.h>
#include <AL/alc.h>

#include "OpenALFileLoader.h"

namespace linguine::audio {

class OpenALAudioManager : public AudioManager {
  public:
    explicit OpenALAudioManager(std::unique_ptr<OpenALFileLoader> fileLoader);

    ~OpenALAudioManager() override;

    void play(EffectType effectType) override;

  private:
    std::unique_ptr<OpenALFileLoader> _fileLoader;

    ALCdevice* _device;
    ALCcontext* _context;

    ALuint _source;

    std::unordered_map<EffectType, ALuint> _buffers;

    void loadBuffer(EffectType effectType);
};

}  // namespace linguine::audio
