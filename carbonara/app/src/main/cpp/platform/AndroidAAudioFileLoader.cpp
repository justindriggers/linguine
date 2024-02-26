#include "AndroidAAudioFileLoader.h"

#include <string>

namespace linguine::carbonara {

std::vector<std::byte> AndroidAAudioFileLoader::getAudioDataForEffect(EffectType effectType) const {
  std::string filename;

  switch (effectType) {
  case EffectType::ButtonDown:
    filename = "select-16bit-44_1khz_1.wav";
    break;
  case EffectType::ButtonUp:
    filename = "select-16bit-44_1khz_2.wav";
    break;
  case EffectType::Collect1:
    filename = "collect1-16bit-44_1khz.wav";
    break;
  case EffectType::Collect2:
    filename = "collect2-16bit-44_1khz.wav";
    break;
  case EffectType::Collect3:
    filename = "collect3-16bit-44_1khz.wav";
    break;
  case EffectType::Collect4:
    filename = "collect4-16bit-44_1khz.wav";
    break;
  case EffectType::Collect5:
    filename = "collect5-16bit-44_1khz.wav";
    break;
  case EffectType::Detonate:
    filename = "detonation-16bit-44_1khz.wav";
    break;
  case EffectType::Explosion:
    filename = "explosion-16bit-44_1khz.wav";
    break;
  case EffectType::Heal:
    filename = "heal-16bit-44_1khz.wav";
    break;
  case EffectType::Level:
    filename = "level-16bit-44_1khz.wav";
    break;
  case EffectType::PowerUp:
    filename = "powerup-16bit-44_1khz.wav";
    break;
  case EffectType::Swoop:
    filename = "swoop-16bit-44_1khz.wav";
    break;
  case EffectType::Xp:
    filename = "xp-16bit-44_1khz.wav";
    break;
  }

  const auto dataHeader = std::string("data");

  auto contents = loadFile(filename);

  auto result = std::vector<std::byte>();

  auto i = 0;
  int32_t dataSize = 0;

  while (i < contents.size() - 4) {
    auto view = std::string_view(reinterpret_cast<const char*>(&contents[i]), dataHeader.size());

    if (view == dataHeader) {
      auto sizeIndex = i + 4;
      dataSize = (dataSize << 8) + static_cast<int32_t>(contents[sizeIndex + 3]);
      dataSize = (dataSize << 8) + static_cast<int32_t>(contents[sizeIndex + 2]);
      dataSize = (dataSize << 8) + static_cast<int32_t>(contents[sizeIndex + 1]);
      dataSize = (dataSize << 8) + static_cast<int32_t>(contents[sizeIndex]);

      result.resize(dataSize);

      auto position = sizeIndex + 4;
      std::copy(contents.begin() + position, contents.begin() + position + dataSize, result.data());

      break;
    }

    ++i;
  }

  return result;
}

std::vector<std::byte> AndroidAAudioFileLoader::getAudioDataForSong(SongType songType) const {
  std::string filename;

  switch (songType) {
  case SongType::Theme:
    filename = "aegis4-16bit-44_1khz.wav";
    break;
  case SongType::Title:
    filename = "aegis4_title-16bit-44_1khz.wav";
    break;
  case SongType::GameOver:
    filename = "aegis4_gameover-16bit-44_1khz.wav";
    break;
  }

  const auto dataHeader = std::string("data");

  auto contents = loadFile(filename);

  auto result = std::vector<std::byte>();

  auto i = 0;
  int32_t dataSize = 0;

  while (i < contents.size() - 4) {
    auto view = std::string_view(reinterpret_cast<const char*>(&contents[i]), dataHeader.size());

    if (view == dataHeader) {
      auto sizeIndex = i + 4;
      dataSize = (dataSize << 8) + static_cast<int32_t>(contents[sizeIndex + 3]);
      dataSize = (dataSize << 8) + static_cast<int32_t>(contents[sizeIndex + 2]);
      dataSize = (dataSize << 8) + static_cast<int32_t>(contents[sizeIndex + 1]);
      dataSize = (dataSize << 8) + static_cast<int32_t>(contents[sizeIndex]);

      result.resize(dataSize);

      auto position = sizeIndex + 4;
      std::copy(contents.begin() + position, contents.begin() + position + dataSize, result.data());

      break;
    }

    ++i;
  }

  return result;
}

std::vector<std::byte> AndroidAAudioFileLoader::loadFile(std::string filename) const {
  auto file = AAssetManager_open(&_assetManager, filename.data(), AASSET_MODE_BUFFER);
  auto size = AAsset_getLength(file);

  auto result = std::vector<std::byte>(size);

  AAsset_read(file, result.data(), result.size());

  return result;
}

}  // namespace linguine::carbonara
