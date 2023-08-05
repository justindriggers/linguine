#pragma once

#include <OpenGLFileLoader.h>

namespace linguine::pesto {

class WebOpenGLFileLoader : public render::OpenGLFileLoader {
  public:
    ~WebOpenGLFileLoader() override = default;

    ImageFile getImage() const override;
};

}  // namespace linguine::pesto
