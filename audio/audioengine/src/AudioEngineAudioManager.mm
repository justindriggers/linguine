#include "AudioEngineAudioManager.h"

#import <AVFoundation/AVAudioMixerNode.h>

namespace linguine::audio {

AudioEngineAudioManager::AudioEngineAudioManager(std::unique_ptr<AudioEngineFileLoader> fileLoader)
    : _fileLoader(std::move(fileLoader)),
      _audioEngine([[AVAudioEngine alloc] init]),
      _playerNodes([[NSMutableArray alloc] init]) {
  auto outputFormat = [_audioEngine.outputNode inputFormatForBus:0];
  auto inputFormat = [[AVAudioFormat alloc] initWithCommonFormat:outputFormat.commonFormat
                                                  sampleRate:outputFormat.sampleRate
                                                    channels:1
                                                 interleaved:outputFormat.isInterleaved];

  [_audioEngine connect:_audioEngine.mainMixerNode
                     to:_audioEngine.outputNode
                 format:outputFormat];

  for (auto i = 0; i < _maxChannels; ++i) {
    auto playerNode = [[AVAudioPlayerNode alloc] init];
    [_playerNodes addObject:playerNode];

    [_audioEngine attachNode:playerNode];
    [_audioEngine connect:playerNode
                       to:_audioEngine.mainMixerNode
                   format:inputFormat];

    _nodePool.push(playerNode);
  }

  NSError* error;
  if (![_audioEngine startAndReturnError:&error]) {
    NSLog(@"%@", error.localizedDescription);
    return;
  }

  for (AVAudioPlayerNode* playerNode in _playerNodes) {
    [playerNode play];
  }
}

AudioEngineAudioManager::~AudioEngineAudioManager() {
  for (AVAudioPlayerNode* playerNode in _playerNodes) {
    [playerNode stop];
  }

  [_audioEngine stop];
}

void AudioEngineAudioManager::play(EffectType effectType) {
  auto playerNode = getPlayerNode();

  if (playerNode) {
    auto file = _fileLoader->getAudioFileForEffect(effectType);

    [playerNode scheduleFile:file
                        atTime:nil
        completionCallbackType:AVAudioPlayerNodeCompletionDataPlayedBack
             completionHandler:^(AVAudioPlayerNodeCompletionCallbackType callbackType) {
               std::unique_lock<std::mutex> lock(_poolMutex);
               _nodePool.push(playerNode);
             }];
  }
}

AVAudioPlayerNode* AudioEngineAudioManager::getPlayerNode() {
  if (_nodePool.empty()) {
    return nullptr;
  }

  std::unique_lock<std::mutex> lock(_poolMutex);

  auto result = _nodePool.front();
  _nodePool.pop();

  return result;
}

}  // namespace linguine::audio
