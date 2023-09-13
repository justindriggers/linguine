#include "IosMetalTextureLoader.h"

#include <Metal/Metal.hpp>

namespace linguine::scampi {

IosMetalTextureLoader::~IosMetalTextureLoader() {
  for (const auto& texture : _textures) {
    texture.second->release();
  }
}

MTL::Texture* IosMetalTextureLoader::getTexture() {
  const auto filename = std::string("font.png");

  auto existing = _textures.find(filename);

  if (existing != _textures.end()) {
    return existing->second;
  }

  const auto delimiter = filename.find('.');
  const auto name = filename.substr(0, delimiter);
  const auto extension = filename.substr(delimiter + 1, filename.size() - delimiter);

  auto url = [[NSBundle mainBundle] URLForResource:[NSString stringWithUTF8String:name.c_str()]
                                     withExtension:[NSString stringWithUTF8String:extension.c_str()]];

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

}  // namespace linguine::scampi
