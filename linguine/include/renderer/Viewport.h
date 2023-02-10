#pragma once

namespace linguine {

class Viewport {
  public:
    void setSize(uint16_t width, uint16_t height) {
      _aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    }

    [[nodiscard]] float getAspectRatio() const {
      return _aspectRatio;
    }

  private:
    float _aspectRatio{};
};

}  // namespace linguine