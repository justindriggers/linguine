#pragma once

#include "Room.h"

namespace linguine {

class RoomB : public Room {
  public:
    RoomB() : Room(11, 7, {4, 7}, {2, -1}, {11, 2}, {-1, 2}, true) {}
};

}  // namespace linguine
