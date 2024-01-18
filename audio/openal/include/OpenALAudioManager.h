#pragma once

#include <audio/AudioManager.h>

#include <queue>

#include <AL/al.h>
#include <AL/alc.h>

#include <TimeManager.h>

#include "OpenALFileLoader.h"

namespace linguine::audio {

class OpenALAudioManager : public AudioManager {
  public:
    explicit OpenALAudioManager(std::unique_ptr<OpenALFileLoader> fileLoader,
                                TimeManager& timeManager);

    ~OpenALAudioManager() override;

    void poll() override;

    void play(EffectType effectType) override;

    void play(SongType songType, Mode mode) override;

    void stopSongs() override;

    void pause() override;

    void resume() override;

  private:
    struct SourceState {
      ALint state;
      ALuint source;
      std::function<void(ALint)> callback;
    };

    struct ScheduledPlayback {
      SongType songType;
      int64_t generation;
      float startTime;

      bool operator>(const ScheduledPlayback& other) const {
        return startTime > other.startTime;
      }
    };

    bool _suspended = false;

    std::unique_ptr<OpenALFileLoader> _fileLoader;
    TimeManager& _timeManager;

    ALCdevice* _device;
    ALCcontext* _context;

    std::array<SourceState, 8> _effectSources{};
    std::array<SourceState, 2> _songSources{};

    std::queue<SourceState*> _effectLruPool{};
    int _currentSongSource = 0;
    float _time = 0.0f;
    float _lastSongStartTime = 0.0f;
    int64_t _generation = 0;

    std::unordered_map<EffectType, ALuint> _effectBuffers;
    std::unordered_map<SongType, ALuint> _songBuffers;

    std::priority_queue<ScheduledPlayback, std::vector<ScheduledPlayback>, std::greater<>> _scheduled;

    void loadBuffer(EffectType effectType);

    void loadBuffer(SongType songType);

    SourceState& getNextSongSource();
};

}  // namespace linguine::audio
