#pragma once

#include "Room.h"

namespace linguine {

class RoomB : public Room {
  public:
    RoomB() : Room(9, 5, {4, 5}, {2, -1}, {9, 2}, {-1, 2}) {}
};

}  // namespace linguine
