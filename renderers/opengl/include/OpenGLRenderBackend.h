#pragma once

#include <renderer/RenderBackend.h>

#include "OpenGLFileLoader.h"

namespace linguine::render {

class OpenGLRenderBackend : public RenderBackend {
  public:
    ~OpenGLRenderBackend() override = default;

    static std::unique_ptr<OpenGLRenderBackend> create(std::unique_ptr<OpenGLFileLoader> fileLoader);

  private:
    friend class OpenGLRenderBackendImpl;

    OpenGLRenderBackend() = default;
};

}  // namespace linguine::render
