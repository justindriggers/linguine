#pragma once

#include <vector>

namespace linguine::render {

class OpenGLFileLoader {
  public:
    struct ImageFile {
      int width;
      int height;
      std::vector<std::byte> data;
    };

    virtual ~OpenGLFileLoader() = default;

    [[nodiscard]] virtual ImageFile getImage() const = 0;
};

}  // namespace linguine::render
