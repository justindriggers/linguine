#pragma once

namespace linguine {

struct Score {
  uint8_t level = 0;
  int32_t points = 0;
  int32_t possiblePoints = 0;
  int32_t mines = 0;
  int32_t possibleMines = 0;
};

}  // namespace linguine
