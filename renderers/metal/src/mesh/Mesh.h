#pragma once

#include <Metal/Metal.hpp>

namespace linguine::render {

class Mesh {
  public:
    virtual ~Mesh() = default;

    virtual void bind(MTL::RenderCommandEncoder& encoder) = 0;
};

}  // namespace linguine::render
