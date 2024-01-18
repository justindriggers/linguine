#include "WebOpenALFileLoader.h"

#include <fstream>
#include <string>
#include <string_view>

namespace linguine::pesto {

std::vector<std::byte> WebOpenALFileLoader::getAudioDataForEffect(EffectType effectType) const {
  std::string filename;

  switch (effectType) {
  case EffectType::ButtonDown:
    filename = "/assets/audio/select-16bit-44_1khz_1.wav";
    break;
  case EffectType::ButtonUp:
    filename = "/assets/audio/select-16bit-44_1khz_2.wav";
    break;
  case EffectType::Collect1:
    filename = "/assets/audio/collect1-16bit-44_1khz.wav";
    break;
  case EffectType::Collect2:
    filename = "/assets/audio/collect2-16bit-44_1khz.wav";
    break;
  case EffectType::Collect3:
    filename = "/assets/audio/collect3-16bit-44_1khz.wav";
    break;
  case EffectType::Collect4:
    filename = "/assets/audio/collect4-16bit-44_1khz.wav";
    break;
  case EffectType::Collect5:
    filename = "/assets/audio/collect5-16bit-44_1khz.wav";
    break;
  case EffectType::Detonate:
    filename = "/assets/audio/detonation-16bit-44_1khz.wav";
    break;
  case EffectType::Explosion:
    filename = "/assets/audio/explosion-16bit-44_1khz.wav";
    break;
  case EffectType::Heal:
    filename = "/assets/audio/heal-16bit-44_1khz.wav";
    break;
  case EffectType::Level:
    filename = "/assets/audio/level-16bit-44_1khz.wav";
    break;
  case EffectType::PowerUp:
    filename = "/assets/audio/powerup-16bit-44_1khz.wav";
    break;
  case EffectType::Swoop:
    filename = "/assets/audio/swoop-16bit-44_1khz.wav";
    break;
  case EffectType::Xp:
    filename = "/assets/audio/xp-16bit-44_1khz.wav";
    break;
  }

  const auto dataHeader = std::string("data");

  auto file = std::ifstream(filename, std::ios::binary | std::ios::ate);
  size_t size = file.tellg();

  auto contents = std::vector<std::byte>(size);

  file.seekg(0, std::ios::beg);
  file.read(reinterpret_cast<char*>(contents.data()), static_cast<int32_t>(size));

  auto result = std::vector<std::byte>();

  auto i = 0;
  int32_t dataSize = 0;

  while (i < size - 4) {
    auto view = std::string_view(reinterpret_cast<const char*>(&contents[i]), dataHeader.size());

    if (view == dataHeader) {
      auto sizeIndex = i + 4;
      dataSize = (dataSize << 8) + static_cast<int32_t>(contents[sizeIndex + 3]);
      dataSize = (dataSize << 8) + static_cast<int32_t>(contents[sizeIndex + 2]);
      dataSize = (dataSize << 8) + static_cast<int32_t>(contents[sizeIndex + 1]);
      dataSize = (dataSize << 8) + static_cast<int32_t>(contents[sizeIndex]);

      result.resize(dataSize);

      file.seekg(sizeIndex + 4, std::ios::beg);
      file.read(reinterpret_cast<char*>(result.data()), dataSize);

      break;
    }

    ++i;
  }

  return result;
}

std::vector<std::byte> WebOpenALFileLoader::getAudioDataForSong(SongType songType) const {
  std::string filename;

  switch (songType) {
  case SongType::Theme:
    filename = "/assets/audio/aegis4-16bit-44_1khz.wav";
    break;
  case SongType::Title:
    filename = "/assets/audio/aegis4_title-16bit-44_1khz.wav";
    break;
  case SongType::GameOver:
    filename = "/assets/audio/aegis4_gameover-16bit-44_1khz.wav";
    break;
  }

  const auto dataHeader = std::string("data");

  auto file = std::ifstream(filename, std::ios::binary | std::ios::ate);
  size_t size = file.tellg();

  auto contents = std::vector<std::byte>(size);

  file.seekg(0, std::ios::beg);
  file.read(reinterpret_cast<char*>(contents.data()), static_cast<int32_t>(size));

  auto result = std::vector<std::byte>();

  auto i = 0;
  int32_t dataSize = 0;

  while (i < size - 4) {
    auto view = std::string_view(reinterpret_cast<const char*>(&contents[i]), dataHeader.size());

    if (view == dataHeader) {
      auto sizeIndex = i + 4;
      dataSize = (dataSize << 8) + static_cast<int32_t>(contents[sizeIndex + 3]);
      dataSize = (dataSize << 8) + static_cast<int32_t>(contents[sizeIndex + 2]);
      dataSize = (dataSize << 8) + static_cast<int32_t>(contents[sizeIndex + 1]);
      dataSize = (dataSize << 8) + static_cast<int32_t>(contents[sizeIndex]);

      result.resize(dataSize);

      file.seekg(sizeIndex + 4, std::ios::beg);
      file.read(reinterpret_cast<char*>(result.data()), dataSize);

      break;
    }

    ++i;
  }

  return result;
}

}  // namespace linguine::pesto
