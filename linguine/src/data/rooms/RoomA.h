#pragma once

#include "Room.h"

namespace linguine {

class RoomA : public Room {
  public:
    RoomA() : Room(9, 11, {2, 11}, {2, -1}, {9, 3}, {-1, 4}, true) {}
};

}  // namespace linguine
