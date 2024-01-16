#include "OpenALAudioManager.h"

#include <AL/al.h>

namespace linguine::audio {

OpenALAudioManager::OpenALAudioManager(std::unique_ptr<OpenALFileLoader> fileLoader,
                                       TimeManager& timeManager)
    : _fileLoader(std::move(fileLoader)), _timeManager(timeManager) {
  _device = alcOpenDevice(nullptr);
  _context = alcCreateContext(_device, nullptr);
  alcMakeContextCurrent(_context);

  alGenSources(static_cast<ALsizei>(_effectSources.size()), _effectSources.data());

  for (auto source : _effectSources) {
    alSourcef(source, AL_PITCH, 1.0f);
    alSourcef(source, AL_GAIN, 1.0f);
    alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    _effectLruPool.push(source);
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

  loadBuffer(EffectType::Pop);
  loadBuffer(EffectType::Select);
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

  alDeleteSources(static_cast<ALsizei>(_effectSources.size()), _effectSources.data());

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

  for (auto& songSource : _songSources) {
    ALint newState;
    alGetSourcei(songSource.source, AL_SOURCE_STATE, &newState);

    if (newState != songSource.state) {
      songSource.state = newState;
      songSource.callback(newState);
    }
  }
}

void OpenALAudioManager::play(EffectType effectType) {
  auto source = _effectLruPool.front();
  _effectLruPool.pop();
  _effectLruPool.push(source);

  alSourcei(source, AL_BUFFER, static_cast<ALint>(_effectBuffers[effectType]));
  alSourcePlay(source);
}

void OpenALAudioManager::play(SongType songType, Mode mode) {
  auto generation = ++_generation;
  _time = 0.0f;
  _lastSongStartTime = 0.0f;
  _scheduled = {};

  for (auto& songSource : _songSources) {
    alSourceStop(songSource.source);
  }


  auto& sourceState = getNextSongSource();
  alSourcei(sourceState.source, AL_BUFFER, static_cast<ALint>(_songBuffers[songType]));
  alSourcePlay(sourceState.source);

  sourceState.callback = [this, generation, songType](ALint newState) {
    if (generation == _generation && newState == AL_STOPPED) {
      _lastSongStartTime += _fileLoader->getSongLoopPoint(songType);
      _scheduled.push({songType, generation, _lastSongStartTime});
    }
  };

  if (mode == Mode::Repeat) {
    _lastSongStartTime += _fileLoader->getSongLoopPoint(songType);
    _scheduled.push({songType, generation, _lastSongStartTime});
  }
}

void OpenALAudioManager::pause() {
  alcSuspendContext(_context);
  _suspended = true;

  alSourcePausev(static_cast<ALsizei>(_effectSources.size()), _effectSources.data());

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

  alSourcePlayv(static_cast<ALsizei>(_effectSources.size()), _effectSources.data());

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

OpenALAudioManager::SongSourceState& OpenALAudioManager::getNextSongSource() {
  if (_currentSongSource == 1) {
    _currentSongSource = 0;
  } else {
    _currentSongSource = 1;
  }

  return _songSources[_currentSongSource];
}

}  // namespace linguine::audio
