#include "AndroidOpenGLFileLoader.h"

#include <string>

#include <android/imagedecoder.h>

namespace linguine::carbonara {

render::OpenGLFileLoader::ImageFile AndroidOpenGLFileLoader::getImage() const {
  const auto filename = std::string("font.png");

  auto image = AAssetManager_open(&_assetManager, filename.data(), AASSET_MODE_BUFFER);

  AImageDecoder* decoder;
  if (AImageDecoder_createFromAAsset(image, &decoder) != ANDROID_IMAGE_DECODER_SUCCESS) {
    return {};
  }

  AImageDecoder_setAndroidBitmapFormat(decoder, ANDROID_BITMAP_FORMAT_RGBA_8888);

  const AImageDecoderHeaderInfo* header;
  header = AImageDecoder_getHeaderInfo(decoder);

  auto width = AImageDecoderHeaderInfo_getWidth(header);
  auto height = AImageDecoderHeaderInfo_getHeight(header);
  auto stride = AImageDecoder_getMinimumStride(decoder);

  auto data = std::vector<std::byte>(height * stride);
  if (AImageDecoder_decodeImage(decoder, data.data(), stride, data.size()) != ANDROID_IMAGE_DECODER_SUCCESS) {
    return {};
  }

  return ImageFile {
      .width = width,
      .height = height,
      .data = data
  };
}

}  // namespace linguine::carbonara
