#include "WebOpenALFileLoader.h"

#include <fstream>
#include <string>

namespace linguine::pesto {

std::vector<std::byte> WebOpenALFileLoader::getAudioDataForEffect(EffectType effectType) const {
  std::string filename;

  switch (effectType) {
  case Pop:
    filename = "/assets/audio/Balloon Pop 1.wav";
    break;
  case Select:
    filename = "/assets/audio/Select 1.wav";
    break;
  }

  auto file = std::ifstream(filename, std::ios::binary | std::ios::ate);
  auto size = file.tellg();

  auto result = std::vector<std::byte>(size);

  file.seekg(0, std::ios::beg);
  file.read(reinterpret_cast<char*>(result.data()), size);

  return result;
}

}  // namespace linguine::pesto
