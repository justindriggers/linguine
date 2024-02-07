#pragma once

namespace linguine {

class Viewport {
  public:
    void setSize(uint16_t width, uint16_t height) {
      _width = width;
      _height = height;
      _aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    }

    void setInsets(uint16_t left, uint16_t right, uint16_t top, uint16_t bottom) {
      _leftInset = left;
      _rightInset = right;
      _topInset = top;
      _bottomInset = bottom;
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

    [[nodiscard]] uint16_t getLeftInset() const {
      return _leftInset;
    }

    [[nodiscard]] uint16_t getRightInset() const {
      return _rightInset;
    }

    [[nodiscard]] uint16_t getTopInset() const {
      return _topInset;
    }

    [[nodiscard]] uint16_t getBottomInset() const {
      return _bottomInset;
    }

  private:
    uint16_t _width{};
    uint16_t _height{};
    float _aspectRatio{};
    uint16_t _leftInset{};
    uint16_t _rightInset{};
    uint16_t _topInset{};
    uint16_t _bottomInset{};
};

}  // namespace linguine