#pragma once

#include "Room.h"

namespace linguine {

class RoomC : public Room {
  public:
    RoomC() : Room(11, 7, {4, 7}, {4, -1}, {11, 3}, {-1, 4}, true) {}
};

}  // namespace linguine
