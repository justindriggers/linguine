#pragma once

#include "Room.h"

namespace linguine {

class SpawnRoom : public Room {
public:
  SpawnRoom() : Room(3, 3, {1, 3}, {1, -1}, {3, 1}, {-1, 1}) {}
};

}  // namespace linguine
