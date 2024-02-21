#include "AndroidOpenGLFileLoader.h"

namespace linguine::carbonara {

render::OpenGLFileLoader::ImageFile AndroidOpenGLFileLoader::getImage() const {
  return ImageFile {
      .width = 0,
      .height = 0,
      .data = std::vector<std::byte>(0)
  };
}

}  // namespace linguine::carbonara
