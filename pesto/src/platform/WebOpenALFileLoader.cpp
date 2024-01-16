#include "WebOpenALFileLoader.h"

#include <fstream>
#include <string>
#include <string_view>

namespace linguine::pesto {

std::vector<std::byte> WebOpenALFileLoader::getAudioDataForEffect(EffectType effectType) const {
  std::string filename;

  switch (effectType) {
  case EffectType::Pop:
    filename = "/assets/audio/Balloon Pop 1.wav";
    break;
  case EffectType::Select:
    filename = "/assets/audio/Select 1.wav";
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
