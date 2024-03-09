#include "AAudioAudioManager.h"

namespace linguine::audio {

aaudio_data_callback_result_t effectCallback([[maybe_unused]] AAudioStream* stream,
                                             void* userData, void* audioData,
                                             int32_t numFrames) {
  auto& streamState = *static_cast<AAudioAudioManager::EffectStreamState*>(userData);

  if (streamState.requestGeneration != streamState.generation) {
    streamState.generation = streamState.requestGeneration;
    streamState.playing = streamState.requested;
    streamState.nextFrame = 0;
    streamState.isPlaying = true;
  }

  if (!streamState.playing) {
    streamState.isPlaying = false;
    return AAUDIO_CALLBACK_RESULT_STOP;
  }

  auto bufferFrames = static_cast<int32_t>(streamState.playing->size() / 4);

  if (bufferFrames > streamState.nextFrame) {
    auto frameCount = bufferFrames - streamState.nextFrame;

    if (frameCount > numFrames) {
      frameCount = numFrames;
    }

    memcpy(audioData, streamState.playing->data() + streamState.nextFrame * 4, frameCount * 4);
    streamState.nextFrame += frameCount;

    return AAUDIO_CALLBACK_RESULT_CONTINUE;
  }

  streamState.isPlaying = false;
  return AAUDIO_CALLBACK_RESULT_STOP;
}

aaudio_data_callback_result_t songCallback([[maybe_unused]] AAudioStream* stream,
                                           void* userData, void* audioData,
                                           int32_t numFrames) {
  auto& streamState = *static_cast<AAudioAudioManager::SongStreamState*>(userData);

  if (streamState.requestGeneration > streamState.generation) {
    streamState.generation = streamState.requestGeneration;
    streamState.playing = streamState.requested;
    streamState.delayFrames = streamState.requestedDelayFrames;
    streamState.loopPoint = streamState.requestedLoopPoint;
    streamState.nextFrame = 0;
    streamState.isPlaying = true;
  }

  if (!streamState.playing) {
    streamState.isPlaying = false;
    return AAUDIO_CALLBACK_RESULT_STOP;
  }

  auto bufferFrames = static_cast<int32_t>(streamState.playing->size() / 4);

  if (bufferFrames > streamState.nextFrame) {
    auto delayFrames = 0;

    if (streamState.nextFrame == 0 && streamState.delayFrames > 0) {
      delayFrames = streamState.delayFrames;

      if (streamState.delayFrames > numFrames) {
        delayFrames = numFrames;
        streamState.delayFrames -= numFrames;
      }

      memset(audioData, 0, delayFrames * 4);

      if (delayFrames >= numFrames) {
        return AAUDIO_CALLBACK_RESULT_CONTINUE;
      } else {
        numFrames -= delayFrames;
        streamState.delayFrames = 0;
      }
    }

    auto frameCount = bufferFrames - streamState.nextFrame;

    if (frameCount > numFrames) {
      frameCount = numFrames;
    } else {
      streamState.delayFrames -= frameCount;
    }

    memcpy(static_cast<std::byte*>(audioData) + delayFrames * 4,
           streamState.playing->data() + streamState.nextFrame * 4,
           frameCount * 4);

    streamState.nextFrame += frameCount;

    return AAUDIO_CALLBACK_RESULT_CONTINUE;
  }

  if (streamState.loopPoint) {
    streamState.nextFrame = 0;
    streamState.delayFrames += *streamState.loopPoint - (static_cast<int32_t>(streamState.playing->size() / 4) - *streamState.loopPoint);
    return AAUDIO_CALLBACK_RESULT_CONTINUE;
  }

  streamState.isPlaying = false;
  return AAUDIO_CALLBACK_RESULT_STOP;
}

AAudioAudioManager::AAudioAudioManager(std::unique_ptr<AAudioFileLoader> fileLoader)
    : _fileLoader(std::move(fileLoader)) {
  AAudioStreamBuilder* builder;
  auto result = AAudio_createStreamBuilder(&builder);

  if (result < 0) {
    AAudioStreamBuilder_delete(builder);
    return;
  }

  AAudioStreamBuilder_setSharingMode(builder, AAUDIO_SHARING_MODE_SHARED);
  AAudioStreamBuilder_setSampleRate(builder, 44100);
  AAudioStreamBuilder_setChannelCount(builder, 2);
  AAudioStreamBuilder_setFormat(builder, AAUDIO_FORMAT_PCM_I16);
  AAudioStreamBuilder_setUsage(builder, AAUDIO_USAGE_GAME);
  AAudioStreamBuilder_setContentType(builder, AAUDIO_CONTENT_TYPE_SONIFICATION);
  AAudioStreamBuilder_setPerformanceMode(builder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);

  for (auto i = 0; i < _effectStreams.size(); ++i) {
    _effectStreamStates[i] = {};
    AAudioStreamBuilder_setDataCallback(builder, effectCallback, &_effectStreamStates[i]);

    result = AAudioStreamBuilder_openStream(builder, &_effectStreams[i]);

    if (result < 0) {
      AAudioStream_close(_effectStreams[i]);
    } else {
      _effectLruPool.push(i);
    }
  }

  AAudioStreamBuilder_setContentType(builder, AAUDIO_CONTENT_TYPE_MUSIC);

  for (auto i = 0; i < _songStreams.size(); ++i) {
    _songStreamStates[i] = {};
    AAudioStreamBuilder_setDataCallback(builder, songCallback, &_songStreamStates[i]);

    result = AAudioStreamBuilder_openStream(builder, &_songStreams[i]);

    if (result < 0) {
      AAudioStream_close(_songStreams[i]);
    }
  }

  AAudioStreamBuilder_delete(builder);

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

AAudioAudioManager::~AAudioAudioManager() {
  for (auto* effectStream : _effectStreams) {
    AAudioStream_close(effectStream);
  }

  for (auto* songStream : _songStreams) {
    AAudioStream_close(songStream);
  }
}

void AAudioAudioManager::setMusicEnabled(bool enabled) {
  _isMusicEnabled = enabled;

  if (!_isMusicEnabled) {
    stopSongs();
  }
}

void AAudioAudioManager::setSoundEffectsEnabled(bool enabled) {
  _isSoundEffectsEnabled = enabled;
}

void AAudioAudioManager::play(EffectType effectType) {
  if (!_isSoundEffectsEnabled) {
    return;
  }

  auto streamIndex = _effectLruPool.front();
  _effectLruPool.pop();
  _effectLruPool.push(streamIndex);

  auto& streamState = _effectStreamStates[streamIndex];
  streamState.requested = &_effectBuffers[effectType];
  ++streamState.requestGeneration;

  AAudioStream_requestStart(_effectStreams[streamIndex]);
}

void AAudioAudioManager::play(SongType songType, Mode mode) {
  if (!_isMusicEnabled) {
    return;
  }

  _currentSongType = songType;

  auto streamIndexA = getNextSongStream();
  auto streamIndexB = getNextSongStream();

  auto* songStreamA = _songStreams[streamIndexA];

  auto& streamStateA = _songStreamStates[streamIndexA];
  auto& streamStateB = _songStreamStates[streamIndexB];

  if (mode == Mode::Repeat) {
    streamStateA.requestedLoopPoint = _fileLoader->getSongLoopPoint(songType);

    streamStateB.requestedDelayFrames = _fileLoader->getSongLoopPoint(songType);
    streamStateB.requestedLoopPoint = _fileLoader->getSongLoopPoint(songType);
    streamStateB.requested = &_songBuffers[songType];
    ++streamStateB.requestGeneration;

    AAudioStream_requestStart(_songStreams[streamIndexB]);
  } else {
    streamStateB.requested = nullptr;
    streamStateA.requestedLoopPoint = {};
  }

  streamStateA.requestedDelayFrames = 0;
  streamStateA.requested = &_songBuffers[songType];
  ++streamStateA.requestGeneration;

  AAudioStream_requestStart(songStreamA);
}

std::optional<SongType> AAudioAudioManager::getCurrentSongType() const {
  return _currentSongType;
}

void AAudioAudioManager::stopSongs() {
  _currentSongType = {};

  for (auto* songStream : _songStreams) {
    AAudioStream_requestStop(songStream);
  }

  auto timeoutNanos = 100L * 1000000L;

  for (auto* songStream : _songStreams) {
    aaudio_stream_state_t nextState = AAUDIO_STREAM_STATE_UNINITIALIZED;
    AAudioStream_waitForStateChange(songStream, AAUDIO_STREAM_STATE_STOPPING,
                                    &nextState, timeoutNanos);
  }
}

void AAudioAudioManager::pause() {
  for (auto i = 0; i < _songStreams.size(); ++i) {
    auto& streamState = _songStreamStates[i];

    if (streamState.isPlaying) {
      AAudioStream_requestPause(_songStreams[i]);
    }
  }

    for (auto i = 0; i < _effectStreams.size(); ++i) {
    auto& streamState = _effectStreamStates[i];

    if (streamState.isPlaying) {
      AAudioStream_requestPause(_effectStreams[i]);
    }
  }
}

void AAudioAudioManager::resume() {
  for (auto i = 0; i < _songStreams.size(); ++i) {
    auto& streamState = _songStreamStates[i];

    if (streamState.isPlaying) {
      AAudioStream_requestStart(_songStreams[i]);
    }
  }

  for (auto i = 0; i < _effectStreams.size(); ++i) {
    auto& streamState = _effectStreamStates[i];

    if (streamState.isPlaying) {
      AAudioStream_requestStart(_effectStreams[i]);
    }
  }
}

void AAudioAudioManager::loadBuffer(EffectType effectType) {
  _effectBuffers[effectType] = _fileLoader->getAudioDataForEffect(effectType);
}

void AAudioAudioManager::loadBuffer(SongType songType) {
  _songBuffers[songType] = _fileLoader->getAudioDataForSong(songType);
}

uint8_t AAudioAudioManager::getNextSongStream() {
  if (_currentSongStream == 1) {
    _currentSongStream = 0;
  } else {
    _currentSongStream = 1;
  }

  return _currentSongStream;
}

}  // namespace linguine::audio