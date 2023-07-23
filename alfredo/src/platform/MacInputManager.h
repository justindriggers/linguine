#pragma once

#include <InputManager.h>

#include <bitset>
#include <unordered_map>

namespace linguine::alfredo {

class MacInputManager : public InputManager {
  public:
    void pollEvents() override;

    [[nodiscard]] const std::unordered_map<uint64_t, Touch>& getTouches() const override;

    [[nodiscard]] float getSensitivity() const override;

    [[nodiscard]] bool isKeyPressed(Key key) const override;

  private:
    std::unordered_map<uint64_t, Touch> _active;
    std::bitset<Key::MAX> _keyStates;
};

}  // namespace linguine::alfredo
