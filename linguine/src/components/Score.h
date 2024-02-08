#pragma once

namespace linguine {

struct Score {
  uint64_t backgroundId{};
  uint64_t iconId{};
  int32_t points = 0;
  int32_t possiblePoints = 0;
  int32_t mines = 0;
  int32_t possibleMines = 0;
};

}  // namespace linguine
