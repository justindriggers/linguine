#pragma once

#include <string>

namespace linguine {

struct Tooltip {
  std::optional<uint64_t> textEntityId{};
  std::optional<uint64_t> backgroundEntityId{};
  bool isEnabled = false;
  bool isDirty = false;

  void setText(std::string text) {
    if (text != _text) {
      _text = std::move(text);
      isDirty = true;
    }

    isEnabled = true;
  }

  std::string& getText() {
    return _text;
  }

  private:
    std::string _text{};
};

}  // namespace linguine
