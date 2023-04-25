#pragma once

#include "Room.h"

namespace linguine {

class RoomC : public Room {
  public:
    RoomC() : Room(9, 5, {4, 5}, {4, -1}, {9, 3}, {-1, 4}) {}
};

}  // namespace linguine
