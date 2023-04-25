#pragma once

#include "Room.h"

namespace linguine {

class RoomA : public Room {
  public:
    RoomA() : Room(7, 9, {2, 9}, {2, -1}, {7, 3}, {-1, 4}) {}
};

}  // namespace linguine
