#pragma once

#include <Metal/Metal.hpp>

namespace linguine::render {

struct MetalRenderContext {
  MTL::Device* device;
  MTL::CommandQueue* commandQueue;
  MTL::CommandBuffer* commandBuffer;
  MTL::RenderPassDescriptor* coloredRenderPassDescriptor;
};

}  // namespace linguine::render
