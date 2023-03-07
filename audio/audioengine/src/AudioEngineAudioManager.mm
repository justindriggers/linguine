#include "AudioEngineAudioManager.h"

#import <AVFoundation/AVAudioMixerNode.h>

namespace linguine::audio {

AudioEngineAudioManager::AudioEngineAudioManager()
    : _audioEngine([[AVAudioEngine alloc] init]),
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

  auto url = [NSURL fileURLWithPath:@"Balloon Pop 1.wav"];
  //auto url = [[NSBundle mainBundle] URLForResource:@"Balloon Pop 1" withExtension:@"wav"];
  _fileToPlay = [[AVAudioFile alloc] initForReading:url
                                              error:&error];

  if (error) {
    NSLog(@"%@", [error localizedDescription]);
    return;
  }
}

AudioEngineAudioManager::~AudioEngineAudioManager() {
  for (AVAudioPlayerNode* playerNode in _playerNodes) {
    [playerNode stop];
  }

  [_audioEngine stop];
}

void AudioEngineAudioManager::play() {
  auto playerNode = getPlayerNode();

  if (playerNode) {
    [playerNode scheduleFile:_fileToPlay
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
