#pragma once

#include <string>

namespace linguine {

struct Upgrade {
  std::string name;
  std::string description;
  std::vector<uint8_t> rankLevels;
};

}  // namespace linguine
