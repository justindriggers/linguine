#pragma once

#include "audio/AudioManager.h"

#include <mutex>
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

    void play(EffectType effectType) override;

  private:
    static constexpr uint8_t _maxChannels = 32;

    std::unique_ptr<AudioEngineFileLoader> _fileLoader;
    AVAudioEngine* _audioEngine;
    NSMutableArray<AVAudioPlayerNode*>* _playerNodes;

    std::queue<AVAudioPlayerNode*> _nodePool;
    std::mutex _poolMutex;

    AVAudioPlayerNode* getPlayerNode();
};

}  // namespace linguine::audio
