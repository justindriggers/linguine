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
      // Owned by main thread
      std::vector<std::byte>* requested;

      // Owned by playback thread
      std::vector<std::byte>* playing;
      int32_t nextFrame;
      bool isPlaying;
    };

    struct SongStreamState {
      // Owned by main thread
      std::vector<std::byte>* requested;
      int32_t requestedDelayFrames;
      std::optional<int32_t> requestedLoopPoint;

      // Owned by playback thread
      std::vector<std::byte>* playing;
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

  private:
    std::unique_ptr<AAudioFileLoader> _fileLoader;

    bool _isMusicEnabled = true;
    bool _isSoundEffectsEnabled = true;

    std::array<AAudioStream*, 8> _effectStreams{};
    std::array<EffectStreamState, 8> _effectStreamStates{};

    std::array<AAudioStream*, 2> _songStreams{};
    std::array<SongStreamState, 2> _songStreamStates{};

    std::queue<uint8_t> _effectLruPool{};
    uint8_t _currentSongStream = 0;
    std::optional<SongType> _currentSongType{};

    std::unordered_map<EffectType, std::vector<std::byte>> _effectBuffers;
    std::unordered_map<SongType, std::vector<std::byte>> _songBuffers;

    void loadBuffer(EffectType effectType);

    void loadBuffer(SongType songType);

    uint8_t getNextSongStream();
};

}  // namespace linguine::audio
