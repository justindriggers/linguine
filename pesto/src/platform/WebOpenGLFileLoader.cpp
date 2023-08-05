#include "WebOpenGLFileLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace linguine::pesto {

render::OpenGLFileLoader::ImageFile WebOpenGLFileLoader::getImage() const {
  auto filename = "/assets/fonts/font.bmp";

  int width, height, components;
  auto data = stbi_load(filename, &width, &height, &components, 0);

  auto size = width * height * components;
  auto dataVector = std::vector<std::byte>(size);

  memcpy(dataVector.data(), data, size);
  stbi_image_free(data);

  return ImageFile {
      .width = width,
      .height = height,
      .data = dataVector
  };
}

}  // namespace linguine::pesto
