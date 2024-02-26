#pragma once

#include <audio/AudioManager.h>

#include <array>
#include <queue>

#include <aaudio/AAudio.h>

#include "AAudioFileLoader.h"

namespace linguine::audio {

class AAudioAudioManager : public AudioManager {
  public:
    struct EffectStreamState {
      std::vector<std::byte>* buffer;
      int32_t nextFrame;
      bool isPlaying;
    };

    struct SongStreamState {
      std::vector<std::byte>* buffer;
      int32_t delayFrames;
      int32_t nextFrame;
      std::optional<int32_t> loopPoint;
      bool isPlaying;
    };

    explicit AAudioAudioManager(std::unique_ptr<AAudioFileLoader> fileLoader);

    ~AAudioAudioManager() override;

    void poll() override {}

    void setMusicEnabled(bool enabled) override;

    void setSoundEffectsEnabled(bool enabled) override;

    void play(EffectType effectType) override;

    void play(SongType songType, Mode mode) override;

    [[nodiscard]] std::optional<SongType> getCurrentSongType() const override;

    void stopSongs() override;

    void pause() override;

    void resume() override;

    EffectStreamState& getEffectStreamState(AAudioStream* stream) {
      return _effectStreamStates.at(stream);
    }

    SongStreamState& getSongStreamState(AAudioStream* stream) {
      return _songStreamStates.at(stream);
    }

  private:
    std::unique_ptr<AAudioFileLoader> _fileLoader;

    bool _isMusicEnabled = true;
    bool _isSoundEffectsEnabled = true;

    std::array<AAudioStream*, 8> _effectStreams{};
    std::array<AAudioStream*, 2> _songStreams{};

    std::queue<AAudioStream*> _effectLruPool{};
    int _currentSongStream = 0;
    std::optional<SongType> _currentSongType{};

    std::unordered_map<EffectType, std::vector<std::byte>> _effectBuffers;
    std::unordered_map<SongType, std::vector<std::byte>> _songBuffers;

    std::unordered_map<AAudioStream*, EffectStreamState> _effectStreamStates;
    std::unordered_map<AAudioStream*, SongStreamState> _songStreamStates;

    void loadBuffer(EffectType effectType);

    void loadBuffer(SongType songType);

    AAudioStream* getNextSongStream();
};

}  // namespace linguine::audio
