#pragma once

#include "renderer/Renderer.h"

#include "OpenGLFileLoader.h"

namespace linguine::render {

class OpenGLRenderer : public Renderer {
  public:
    ~OpenGLRenderer() override = default;

    static OpenGLRenderer* create(std::unique_ptr<OpenGLFileLoader> fileLoader);

  private:
    friend class OpenGLRendererImpl;

    OpenGLRenderer() = default;
};

}  // namespace linguine::render
