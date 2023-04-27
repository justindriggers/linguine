#pragma once

#include "Room.h"

namespace linguine {

class SpawnRoom : public Room {
public:
  SpawnRoom() : Room(5, 5, {2, 5}, {2, -1}, {5, 2}, {-1, 2}, false) {}
};

}  // namespace linguine
