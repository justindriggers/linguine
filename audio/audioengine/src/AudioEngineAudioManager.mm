#include "AudioEngineAudioManager.h"

#import <AVFoundation/AVAudioMixerNode.h>

namespace linguine::audio {

AudioEngineAudioManager::AudioEngineAudioManager(std::unique_ptr<AudioEngineFileLoader> fileLoader)
    : _fileLoader(std::move(fileLoader)),
      _audioEngine([[AVAudioEngine alloc] init]),
      _playerNodes([[NSMutableArray alloc] init]) {
  auto outputFormat = [_audioEngine.outputNode inputFormatForBus:0];

  [_audioEngine connect:_audioEngine.mainMixerNode
                     to:_audioEngine.outputNode
                 format:outputFormat];

  initSongNodes();
  initEffectNodes();

  NSError* error;
  if (![_audioEngine startAndReturnError:&error]) {
    NSLog(@"%@", error.localizedDescription);
    return;
  }

  loadBuffer(EffectType::ButtonDown);
  loadBuffer(EffectType::ButtonUp);
  loadBuffer(EffectType::Collect1);
  loadBuffer(EffectType::Collect2);
  loadBuffer(EffectType::Collect3);
  loadBuffer(EffectType::Collect4);
  loadBuffer(EffectType::Collect5);
  loadBuffer(EffectType::Detonate);
  loadBuffer(EffectType::Explosion);
  loadBuffer(EffectType::Heal);
  loadBuffer(EffectType::Level);
  loadBuffer(EffectType::PowerUp);
  loadBuffer(EffectType::Swoop);
  loadBuffer(EffectType::Xp);
  loadBuffer(SongType::Theme);
  loadBuffer(SongType::Title);
  loadBuffer(SongType::GameOver);
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
    if (!playerNode.isPlaying) {
      [playerNode play];
    }

    [playerNode scheduleBuffer:_effectBuffers[effectType]
                        atTime:nil
                       options:AVAudioPlayerNodeBufferInterrupts
        completionCallbackType:AVAudioPlayerNodeCompletionDataPlayedBack
             completionHandler:nil];
  }
}

void AudioEngineAudioManager::play(SongType songType, Mode mode) {
  stopSongs();

  auto generation = _generation;

  auto songNode = getNextSongNode();

  auto buffer = _songBuffers[songType];

  _lastSongStartSample = 0;
  [songNode scheduleBuffer:buffer
                      atTime:[AVAudioTime timeWithSampleTime:_lastSongStartSample atRate:buffer.format.sampleRate]
                     options:0
      completionCallbackType:AVAudioPlayerNodeCompletionDataPlayedBack
           completionHandler:^(AVAudioPlayerNodeCompletionCallbackType callbackType) {
             if (generation == _generation && mode == Mode::Repeat) {
               loop(songType, generation);
             }
           }];

  auto repeatSongNode = getNextSongNode();

  if (mode == Mode::Repeat) {
    _lastSongStartSample += _fileLoader->getSongLoopPoint(songType);
    [repeatSongNode scheduleBuffer:buffer
                        atTime:[AVAudioTime timeWithSampleTime:_lastSongStartSample atRate:buffer.format.sampleRate]
                       options:0
          completionCallbackType:AVAudioPlayerNodeCompletionDataPlayedBack
               completionHandler:^(AVAudioPlayerNodeCompletionCallbackType callbackType) {
                 if (generation == _generation) {
                   loop(songType, generation);
                 }
               }];
  }
}

void AudioEngineAudioManager::stopSongs() {
  ++_generation;

  for (auto songNode : _songNodes) {
    if (songNode.isPlaying) {
      [songNode stop];
    }

    [songNode play];
  }
}

void AudioEngineAudioManager::pause() {
  for (AVAudioPlayerNode* playerNode in _playerNodes) {
    [playerNode pause];
  }

  [_audioEngine pause];
}

void AudioEngineAudioManager::resume() {
  NSError* error;
  if (![_audioEngine startAndReturnError:&error]) {
    NSLog(@"%@", error.localizedDescription);
    return;
  }

  for (AVAudioPlayerNode* playerNode in _playerNodes) {
    [playerNode play];
  }
}

void AudioEngineAudioManager::initSongNodes() {
  for (auto& songNode : _songNodes) {
    songNode = [[AVAudioPlayerNode alloc] init];
    [_playerNodes addObject:songNode];

    [_audioEngine attachNode:songNode];
    [_audioEngine connect:songNode
                       to:_audioEngine.mainMixerNode
                   format:[[AVAudioFormat alloc] initStandardFormatWithSampleRate:44100
                                                                         channels:2]];
  }
}

void AudioEngineAudioManager::initEffectNodes() {
  for (auto i = 0; i < _maxEffectChannels; ++i) {
    auto playerNode = [[AVAudioPlayerNode alloc] init];
    [_playerNodes addObject:playerNode];

    [_audioEngine attachNode:playerNode];
    [_audioEngine connect:playerNode
                       to:_audioEngine.mainMixerNode
                   format:[[AVAudioFormat alloc] initStandardFormatWithSampleRate:44100
                                                                         channels:2]];

    _effectLruPool.push(playerNode);
  }
}

void AudioEngineAudioManager::loadBuffer(EffectType effectType) {
  auto file = _fileLoader->getAudioFileForEffect(effectType);

  auto buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:file.processingFormat
                                              frameCapacity:static_cast<AVAudioFrameCount>(file.length)];

  NSError* error;
  if (![file readIntoBuffer:buffer error:&error]) {
    NSLog(@"%@", error.localizedDescription);
    return;
  }

  _effectBuffers[effectType] = buffer;
}

void AudioEngineAudioManager::loadBuffer(SongType songType) {
  auto file = _fileLoader->getAudioFileForSong(songType);

  auto buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:file.processingFormat
                                              frameCapacity:static_cast<AVAudioFrameCount>(file.length)];

  NSError* error;
  if (![file readIntoBuffer:buffer error:&error]) {
    NSLog(@"%@", error.localizedDescription);
    return;
  }

  _songBuffers[songType] = buffer;
}

AVAudioPlayerNode* AudioEngineAudioManager::getPlayerNode() {
  auto result = _effectLruPool.front();
  _effectLruPool.pop();
  _effectLruPool.push(result);

  return result;
}

AVAudioPlayerNode* AudioEngineAudioManager::getNextSongNode() {
  if (_currentSongNode == 1) {
    _currentSongNode = 0;
  } else {
    _currentSongNode = 1;
  }

  return _songNodes[_currentSongNode];
}

void AudioEngineAudioManager::loop(SongType songType, int64_t generation) {
  auto songNode = getNextSongNode();

  _lastSongStartSample += _fileLoader->getSongLoopPoint(songType);
  auto buffer = _songBuffers[songType];
  [songNode scheduleBuffer:buffer
                      atTime:[AVAudioTime timeWithSampleTime:_lastSongStartSample atRate:buffer.format.sampleRate]
                     options:0
      completionCallbackType:AVAudioPlayerNodeCompletionDataPlayedBack
           completionHandler:^(AVAudioPlayerNodeCompletionCallbackType callbackType) {
             if (generation == _generation) {
               loop(songType, generation);
             }
           }];
}

}  // namespace linguine::audio
