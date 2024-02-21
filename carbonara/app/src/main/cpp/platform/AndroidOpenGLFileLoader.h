#pragma once

#include <OpenGLFileLoader.h>

namespace linguine::carbonara {

class AndroidOpenGLFileLoader : public render::OpenGLFileLoader {
  public:
    ~AndroidOpenGLFileLoader() override = default;

    [[nodiscard]] ImageFile getImage() const override;
};

}  // namespace linguine::carbonara
