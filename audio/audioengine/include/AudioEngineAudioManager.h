#pragma once

#include "audio/AudioManager.h"

#include <queue>

#import <AVFoundation/AVAudioEngine.h>
#import <AVFoundation/AVAudioFile.h>
#import <AVFoundation/AVAudioPlayerNode.h>

#include "AudioEngineFileLoader.h"

namespace linguine::audio {

class AudioEngineAudioManager : public AudioManager {
  public:
    explicit AudioEngineAudioManager(std::unique_ptr<AudioEngineFileLoader> fileLoader);

    ~AudioEngineAudioManager() override;

    void poll() override {};

    void setMusicEnabled(bool enabled) override;

    void setSoundEffectsEnabled(bool enabled) override;

    void play(EffectType effectType) override;

    void play(SongType songType, Mode mode) override;

    [[nodiscard]] std::optional<SongType> getCurrentSongType() const override;

    void stopSongs() override;

    void pause() override;

    void resume() override;

  private:
    static constexpr uint8_t _maxEffectChannels = 8;

    std::unique_ptr<AudioEngineFileLoader> _fileLoader;
    AVAudioEngine* _audioEngine;
    NSMutableArray<AVAudioPlayerNode*>* _playerNodes;

    bool _isMusicEnabled = true;
    bool _isSoundEffectsEnabled = true;

    std::array<AVAudioPlayerNode*, 2> _songNodes;
    int _currentSongNode = 0;
    std::optional<SongType> _currentSongType{};
    int64_t _lastSongStartSample = 0;
    int64_t _generation = 0;

    std::unordered_map<EffectType, AVAudioPCMBuffer*> _effectBuffers;
    std::unordered_map<SongType, AVAudioPCMBuffer*> _songBuffers;

    std::queue<AVAudioPlayerNode*> _effectLruPool;

    void initSongNodes();

    void initEffectNodes();

    void loadBuffer(EffectType effectType);

    void loadBuffer(SongType songType);

    AVAudioPlayerNode* getPlayerNode();

    AVAudioPlayerNode* getNextSongNode();

    void loop(SongType songType, int64_t generation);
};

}  // namespace linguine::audio
