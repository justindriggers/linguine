#include "MacMetalTextureLoader.h"

#include <Metal/Metal.hpp>

namespace linguine::alfredo {

MacMetalTextureLoader::~MacMetalTextureLoader() {
  for (const auto& texture : _textures) {
    texture.second->release();
  }
}

MTL::Texture* MacMetalTextureLoader::getTexture() {
  const auto filename = "font.bmp";

  auto existing = _textures.find(filename);

  if (existing != _textures.end()) {
    return existing->second;
  }

  auto url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:filename]
                        isDirectory:false];

  NSError* error;
  auto result = [_mtkTextureLoader newTextureWithContentsOfURL:url
                                              options:nil
                                                error:&error];

  if (!result) {
    NSLog(@"%@", [error localizedDescription]);
    return nullptr;
  }

  _textures[filename] = (__bridge_retained MTL::Texture*) result;

  return _textures.at(filename);
}

}  // namespace linguine::alfredo
