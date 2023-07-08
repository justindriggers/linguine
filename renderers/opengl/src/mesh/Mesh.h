#pragma once

namespace linguine::render {

class Mesh {
  public:
    virtual ~Mesh() = default;

    virtual void bind() = 0;

    virtual void unbind() = 0;

    virtual void draw() = 0;
};

}  // namespace linguine::render
