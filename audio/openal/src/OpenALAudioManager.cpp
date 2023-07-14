#include "OpenALAudioManager.h"

#include <AL/al.h>

namespace linguine::audio {

OpenALAudioManager::OpenALAudioManager(std::unique_ptr<OpenALFileLoader> fileLoader)
    : _fileLoader(std::move(fileLoader)) {
  _device = alcOpenDevice(nullptr);
  _context = alcCreateContext(_device, nullptr);
  alcMakeContextCurrent(_context);

  alGenSources(1, &_source);

  alSourcef(_source, AL_PITCH, 1.0f);
  alSourcef(_source, AL_GAIN, 1.0f);
  alSource3f(_source, AL_POSITION, 0.0f, 0.0f, 0.0f);
  alSource3f(_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
  alSourcei(_source, AL_LOOPING, AL_FALSE);

  loadBuffer(EffectType::Pop);
  loadBuffer(EffectType::Select);
}

OpenALAudioManager::~OpenALAudioManager() {
  for (const auto& buffer : _buffers) {
    alDeleteBuffers(1, &buffer.second);
  }

  alDeleteSources(1, &_source);

  alcMakeContextCurrent(nullptr);
  alcDestroyContext(_context);
  alcCloseDevice(_device);
}

void OpenALAudioManager::play(EffectType effectType) {
  alSourcei(_source, AL_BUFFER, _buffers[effectType]);
  alSourcePlay(_source);
}

void OpenALAudioManager::loadBuffer(EffectType effectType) {
  ALuint buffer;
  alGenBuffers(1, &buffer);

  auto data = _fileLoader->getAudioDataForEffect(effectType);
  alBufferData(buffer, AL_FORMAT_STEREO16, data.data(), data.size(), 44100);

  _buffers[effectType] = buffer;
}

}  // namespace linguine::audio
