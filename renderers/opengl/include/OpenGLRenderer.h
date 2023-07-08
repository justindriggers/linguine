#pragma once

#include "renderer/Renderer.h"

namespace linguine::render {

class OpenGLRenderer : public Renderer {
  public:
    ~OpenGLRenderer() override = default;

    static OpenGLRenderer* create();

  private:
    friend class OpenGLRendererImpl;

    OpenGLRenderer() = default;
};

}  // namespace linguine::render
