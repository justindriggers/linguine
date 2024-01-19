#include "OpenALAudioManager.h"

#include <AL/al.h>

namespace linguine::audio {

OpenALAudioManager::OpenALAudioManager(std::unique_ptr<OpenALFileLoader> fileLoader,
                                       TimeManager& timeManager)
    : _fileLoader(std::move(fileLoader)), _timeManager(timeManager) {
  _device = alcOpenDevice(nullptr);
  _context = alcCreateContext(_device, nullptr);
  alcMakeContextCurrent(_context);

  for (auto& effectSource : _effectSources) {
    alGenSources(1, &effectSource.source);

    effectSource.state = AL_STOPPED;
    effectSource.callback = [](ALint newState) {};

    alSourcef(effectSource.source, AL_PITCH, 1.0f);
    alSourcef(effectSource.source, AL_GAIN, 1.0f);
    alSource3f(effectSource.source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(effectSource.source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(effectSource.source, AL_LOOPING, AL_FALSE);

    _effectLruPool.push(&effectSource);
  }

  for (auto& songSource : _songSources) {
    alGenSources(1, &songSource.source);

    songSource.state = AL_STOPPED;
    songSource.callback = [](ALint newState) {};

    alSourcef(songSource.source, AL_PITCH, 1.0f);
    alSourcef(songSource.source, AL_GAIN, 1.0f);
    alSource3f(songSource.source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(songSource.source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(songSource.source, AL_LOOPING, AL_FALSE);
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

OpenALAudioManager::~OpenALAudioManager() {
  for (const auto& buffer : _effectBuffers) {
    alDeleteBuffers(1, &buffer.second);
  }

  for (const auto& buffer : _songBuffers) {
    alDeleteBuffers(1, &buffer.second);
  }

  for (auto& effectSource : _effectSources) {
    alDeleteSources(1, &effectSource.source);
  }

  for (auto& songSource : _songSources) {
    alDeleteSources(1, &songSource.source);
  }

  alcMakeContextCurrent(nullptr);
  alcDestroyContext(_context);
  alcCloseDevice(_device);
}

void OpenALAudioManager::poll() {
  if (_suspended) {
    return;
  }

  _time += _timeManager.getTimeStep();

  while (!_scheduled.empty() && _time >= _scheduled.top().startTime) {
    auto& scheduled = _scheduled.top();

    auto songType = scheduled.songType;
    auto generation = scheduled.generation;

    auto& sourceState = getNextSongSource();
    alSourcei(sourceState.source, AL_BUFFER, static_cast<ALint>(_songBuffers[songType]));
    alSourcePlay(sourceState.source);

    sourceState.callback = [this, generation, songType](ALint newState) {
      if (generation == _generation && newState == AL_STOPPED) {
        _lastSongStartTime += _fileLoader->getSongLoopPoint(songType);
        _scheduled.push({songType, generation, _lastSongStartTime});
      }
    };

    _scheduled.pop();
  }

  for (auto& effectSource : _effectSources) {
    ALint newState;
    alGetSourcei(effectSource.source, AL_SOURCE_STATE, &newState);

    if (newState != effectSource.state) {
      effectSource.state = newState;
      effectSource.callback(newState);
    }
  }

  for (auto& songSource : _songSources) {
    ALint newState;
    alGetSourcei(songSource.source, AL_SOURCE_STATE, &newState);

    if (newState != songSource.state) {
      songSource.state = newState;
      songSource.callback(newState);
    }
  }
}

void OpenALAudioManager::setMusicEnabled(bool enabled) {
  _isMusicEnabled = enabled;

  if (!_isMusicEnabled) {
    stopSongs();
  }
}

void OpenALAudioManager::setSoundEffectsEnabled(bool enabled) {
  _isSoundEffectsEnabled = enabled;
}

void OpenALAudioManager::play(EffectType effectType) {
  if (!_isSoundEffectsEnabled) {
    return;
  }

  auto effectSource = _effectLruPool.front();
  _effectLruPool.pop();
  _effectLruPool.push(effectSource);

  alSourceStop(effectSource->source);

  alSourcei(effectSource->source, AL_BUFFER, static_cast<ALint>(_effectBuffers[effectType]));
  alSourcePlay(effectSource->source);
}

void OpenALAudioManager::play(SongType songType, Mode mode) {
  if (!_isMusicEnabled) {
    return;
  }

  stopSongs();

  auto generation = _generation;
  _currentSongType = songType;

  auto& sourceState = getNextSongSource();
  alSourcei(sourceState.source, AL_BUFFER, static_cast<ALint>(_songBuffers[songType]));
  alSourcePlay(sourceState.source);

  sourceState.callback = [this, generation, songType, mode](ALint newState) {
    if (generation == _generation && newState == AL_STOPPED) {
      _lastSongStartTime += _fileLoader->getSongLoopPoint(songType);
      _scheduled.push({songType, generation, _lastSongStartTime});
    }

    if (mode != Mode::Repeat) {
      _currentSongType = {};
    }
  };

  if (mode == Mode::Repeat) {
    _lastSongStartTime += _fileLoader->getSongLoopPoint(songType);
    _scheduled.push({songType, generation, _lastSongStartTime});
  }
}

std::optional<SongType> OpenALAudioManager::getCurrentSongType() const {
  return _currentSongType;
}

void OpenALAudioManager::stopSongs() {
  ++_generation;
  _currentSongType = {};

  _time = 0.0f;
  _lastSongStartTime = 0.0f;
  _scheduled = {};

  for (auto& songSource : _songSources) {
    alSourceStop(songSource.source);
  }
}

void OpenALAudioManager::pause() {
  alcSuspendContext(_context);
  _suspended = true;

  for (auto& effectSource : _effectSources) {
    if (effectSource.state == AL_PLAYING) {
      alSourcePause(effectSource.source);
    }
  }

  for (auto& songSource : _songSources) {
    if (songSource.state == AL_PLAYING) {
      alSourcePause(songSource.source);
    }
  }
}

void OpenALAudioManager::resume() {
  if (!_suspended) {
    return;
  }

  alcProcessContext(_context);
  _suspended = false;

  for (auto& effectSource : _effectSources) {
    if (effectSource.state == AL_PLAYING) {
      alSourcePlay(effectSource.source);
    }
  }

  for (auto& songSource : _songSources) {
    if (songSource.state == AL_PLAYING) {
      alSourcePlay(songSource.source);
    }
  }
}

void OpenALAudioManager::loadBuffer(EffectType effectType) {
  ALuint buffer;
  alGenBuffers(1, &buffer);

  auto data = _fileLoader->getAudioDataForEffect(effectType);
  alBufferData(buffer, AL_FORMAT_STEREO16, data.data(), static_cast<ALsizei>(data.size()), 44100);

  _effectBuffers[effectType] = buffer;
}

void OpenALAudioManager::loadBuffer(SongType songType) {
  ALuint buffer;
  alGenBuffers(1, &buffer);

  auto data = _fileLoader->getAudioDataForSong(songType);
  alBufferData(buffer, AL_FORMAT_STEREO16, data.data(), static_cast<ALsizei>(data.size()), 44100);

  _songBuffers[songType] = buffer;
}

OpenALAudioManager::SourceState& OpenALAudioManager::getNextSongSource() {
  if (_currentSongSource == 1) {
    _currentSongSource = 0;
  } else {
    _currentSongSource = 1;
  }

  return _songSources[_currentSongSource];
}

}  // namespace linguine::audio
