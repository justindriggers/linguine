#pragma once

namespace linguine {

class Viewport {
  public:
    void setSize(uint16_t width, uint16_t height) {
      _width = width;
      _height = height;
      _aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    }

    [[nodiscard]] float getAspectRatio() const {
      return _aspectRatio;
    }

    [[nodiscard]] uint16_t getWidth() const {
      return _width;
    }

    [[nodiscard]] uint16_t getHeight() const {
      return _height;
    }

  private:
    uint16_t _width{};
    uint16_t _height{};
    float _aspectRatio{};
};

}  // namespace linguine