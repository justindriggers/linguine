#pragma once

#include <cstdint>

namespace linguine {

class SaveManager {
  public:
    SaveManager() = default;

    virtual ~SaveManager() = default;

    void addPoints(int32_t points) {
      _points += points;
      save();
    }

    void removePoints(int32_t points) {
      _points -= points;
      save();
    }

    [[nodiscard]] int32_t getPoints() const {
      return _points;
    }

    [[nodiscard]] bool isNewPlayer() const {
      return _points == 0;
    }

    void restart() {
      _points = 0;
      save();
    }

  protected:
    int32_t _points{};

    virtual void load() = 0;

    virtual void save() = 0;
};

}  // namespace linguine
