#pragma once

#include <AudioManager.h>

#include <mutex>
#include <queue>

#import <AVFoundation/AVAudioEngine.h>
#import <AVFoundation/AVAudioFile.h>
#import <AVFoundation/AVAudioPlayerNode.h>

namespace linguine::audio {

class AudioEngineAudioManager : public AudioManager {
  public:
    AudioEngineAudioManager();

    ~AudioEngineAudioManager() override;

    void play() override;

  private:
    static constexpr uint8_t _maxChannels = 32;

    AVAudioEngine* _audioEngine;
    AVAudioFile* _fileToPlay;

    NSMutableArray<AVAudioPlayerNode*>* _playerNodes;

    std::queue<AVAudioPlayerNode*> _nodePool;
    std::mutex _poolMutex;

    AVAudioPlayerNode* getPlayerNode();
};

}  // namespace linguine::audio
