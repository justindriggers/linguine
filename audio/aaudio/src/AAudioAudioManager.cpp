#include "AAudioAudioManager.h"

namespace linguine::audio {

aaudio_data_callback_result_t effectCallback(AAudioStream* stream, void* userData,
                                             void* audioData, int32_t numFrames) {
  auto* audioManager = static_cast<AAudioAudioManager*>(userData);
  auto& streamState = audioManager->getEffectStreamState(stream);

  auto* buffer = streamState.buffer;
  auto bufferFrames = static_cast<int32_t>(buffer->size() / 4);

  if (bufferFrames > streamState.nextFrame) {
    auto frameCount = bufferFrames - streamState.nextFrame;

    if (frameCount > numFrames) {
      frameCount = numFrames;
    }

    memcpy(audioData, buffer->data() + streamState.nextFrame * 4, frameCount * 4);
    streamState.nextFrame += frameCount;

    return AAUDIO_CALLBACK_RESULT_CONTINUE;
  }

  streamState.isPlaying = false;
  return AAUDIO_CALLBACK_RESULT_STOP;
}

aaudio_data_callback_result_t songCallback(AAudioStream* stream, void* userData,
                                           void* audioData, int32_t numFrames) {
  auto* audioManager = static_cast<AAudioAudioManager*>(userData);
  auto& streamState = audioManager->getSongStreamState(stream);

  auto* buffer = streamState.buffer;
  auto bufferFrames = static_cast<int32_t>(buffer->size() / 4);

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
           buffer->data() + streamState.nextFrame * 4,
           frameCount * 4);

    streamState.nextFrame += frameCount;

    return AAUDIO_CALLBACK_RESULT_CONTINUE;
  }

  if (streamState.loopPoint) {
    streamState.nextFrame = 0;
    streamState.delayFrames += *streamState.loopPoint - (static_cast<int32_t>(streamState.buffer->size() / 4) - *streamState.loopPoint);
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
  AAudioStreamBuilder_setDataCallback(builder, effectCallback, this);

  for (auto* effectStream : _effectStreams) {
    result = AAudioStreamBuilder_openStream(builder, &effectStream);

    if (result < 0) {
      AAudioStream_close(effectStream);
    } else {
      _effectStreamStates[effectStream] = {};
      _effectLruPool.push(effectStream);
    }
  }

  AAudioStreamBuilder_setContentType(builder, AAUDIO_CONTENT_TYPE_MUSIC);
  AAudioStreamBuilder_setDataCallback(builder, songCallback, this);

  for (auto& songStream : _songStreams) {
    result = AAudioStreamBuilder_openStream(builder, &songStream);

    if (result < 0) {
      AAudioStream_close(songStream);
    }

    _songStreamStates[songStream] = {};
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

  auto effectStream = _effectLruPool.front();
  _effectLruPool.pop();
  _effectLruPool.push(effectStream);

  auto& streamState = getEffectStreamState(effectStream);

  int64_t timeoutNanos = 10L * 1'000'000L;

  AAudioStream_requestStop(effectStream);
  AAudioStream_waitForStateChange(effectStream, AAUDIO_STREAM_STATE_STOPPING,
                                  nullptr, timeoutNanos);

  streamState.buffer = &_effectBuffers[effectType];
  streamState.nextFrame = 0;
  streamState.isPlaying = true;

  AAudioStream_requestStart(effectStream);
}

void AAudioAudioManager::play(SongType songType, Mode mode) {
  if (!_isMusicEnabled) {
    return;
  }

  stopSongs();

  _currentSongType = songType;

  auto songStreamA = getNextSongStream();
  auto& streamStateA = getSongStreamState(songStreamA);

  streamStateA.buffer = &_songBuffers[songType];
  streamStateA.delayFrames = 0;
  streamStateA.nextFrame = 0;
  streamStateA.isPlaying = true;

  if (mode == Mode::Repeat) {
    streamStateA.loopPoint = _fileLoader->getSongLoopPoint(songType);

    auto songStreamB = getNextSongStream();

    auto& streamStateB = getSongStreamState(songStreamB);
    streamStateB.buffer = &_songBuffers[songType];
    streamStateB.delayFrames = _fileLoader->getSongLoopPoint(songType);
    streamStateB.nextFrame = 0;
    streamStateB.isPlaying = true;
    streamStateB.loopPoint = _fileLoader->getSongLoopPoint(songType);

    AAudioStream_requestStart(songStreamB);
  } else {
    streamStateA.loopPoint = {};
  }

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
  for (auto* songStream : _songStreams) {
    auto& streamState = getSongStreamState(songStream);

    if (streamState.isPlaying) {
      AAudioStream_requestPause(songStream);
    }
  }

  for (auto* effectStream : _effectStreams) {
    auto& streamState = getEffectStreamState(effectStream);

    if (streamState.isPlaying) {
      AAudioStream_requestPause(effectStream);
    }
  }
}

void AAudioAudioManager::resume() {
  for (auto* songStream : _songStreams) {
    auto& streamState = getSongStreamState(songStream);

    if (streamState.isPlaying) {
      AAudioStream_requestStart(songStream);
    }
  }

  for (auto* effectStream : _effectStreams) {
    auto& streamState = getEffectStreamState(effectStream);

    if (streamState.isPlaying) {
      AAudioStream_requestStart(effectStream);
    }
  }
}

void AAudioAudioManager::loadBuffer(EffectType effectType) {
  _effectBuffers[effectType] = _fileLoader->getAudioDataForEffect(effectType);
}

void AAudioAudioManager::loadBuffer(SongType songType) {
  _songBuffers[songType] = _fileLoader->getAudioDataForSong(songType);
}

AAudioStream* AAudioAudioManager::getNextSongStream() {
  if (_currentSongStream == 1) {
    _currentSongStream = 0;
  } else {
    _currentSongStream = 1;
  }

  return _songStreams[_currentSongStream];
}

}  // namespace linguine::audio