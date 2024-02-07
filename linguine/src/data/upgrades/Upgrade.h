#pragma once

#include <string>

namespace linguine {

struct Upgrade {
  enum Type : uint8_t {
    Shield = 0,
    Durability = 1,
    Speed = 2,
    Acceleration = 3
  };

  std::string name;
  std::string description;
  std::vector<uint8_t> rankLevels;
};

}  // namespace linguine
