#pragma once

#include <Metal/Metal.hpp>

namespace linguine::render {

struct MetalRenderContext {
  MTL::Device* device = nullptr;
  MTL::CommandQueue* commandQueue = nullptr;
  MTL::CommandBuffer* commandBuffer = nullptr;
  MTL::RenderPassDescriptor* coloredRenderPassDescriptor = nullptr;
};

}  // namespace linguine::render
