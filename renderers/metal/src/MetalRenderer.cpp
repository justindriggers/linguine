#include "MetalRenderer.h"

#include <simd/simd.h>

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION

#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>

class linguine::render::MetalRendererImpl : public linguine::render::MetalRenderer {
  public:
    explicit MetalRendererImpl(MTK::View& view) : _view(view) {
      _view.setColorPixelFormat(MTL::PixelFormatBGRA8Unorm_sRGB);
      _view.setClearColor(MTL::ClearColor::Make(0.0f, 0.0f, 0.0f, 1.0f));

      _device = _view.device();

      _commandQueue = _device->newCommandQueue();

      buildBuffers();
      buildShaders();
    }

    ~MetalRendererImpl() override {
      _vertexPositionsBuffer->release();
      _vertexColorsBuffer->release();
      _pipelineState->release();
      _commandQueue->release();
      _device->release();
    }

    void draw() override;

  private:
    MTK::View& _view;
    MTL::Device* _device = nullptr;
    MTL::CommandQueue* _commandQueue = nullptr;

    MTL::Buffer* _vertexPositionsBuffer = nullptr;
    MTL::Buffer* _vertexColorsBuffer = nullptr;

    MTL::RenderPipelineState* _pipelineState = nullptr;

    void buildBuffers();

    void buildShaders();
};

void linguine::render::MetalRendererImpl::buildBuffers() {
    simd::float2 positions[] = {
        { -0.5f, -0.5f },
        {  0.0f,  0.5f },
        {  0.5f, -0.5f }
    };

    const auto positionsBufferSize = std::size(positions) * sizeof(simd::float2);
    _vertexPositionsBuffer = _device->newBuffer(positionsBufferSize, MTL::ResourceStorageModeShared);

    memcpy(_vertexPositionsBuffer->contents(), positions, positionsBufferSize);
    _vertexPositionsBuffer->didModifyRange(NS::Range::Make(0, positionsBufferSize));

    simd::float3 colors[] = {
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f }
    };

    const auto colorBufferSize = std::size(colors) * sizeof(simd::float3);
    _vertexColorsBuffer = _device->newBuffer(colorBufferSize, MTL::ResourceStorageModeShared);

    memcpy(_vertexColorsBuffer->contents(), colors, colorBufferSize);
    _vertexColorsBuffer->didModifyRange(NS::Range::Make(0, colorBufferSize));
}

void linguine::render::MetalRendererImpl::buildShaders() {
    const auto shaderSourceCode = R"(
      struct VertexOutput {
        float4 position [[position]];
        half3 color;
      };

      VertexOutput vertex vertexColored(uint index [[vertex_id]],
          const device float2* positions [[buffer(0)]],
          const device float3* colors [[buffer(1)]]) {
        VertexOutput o;
        o.position = float4(positions[index], 0.0, 1.0);
        o.color = half3(colors[index]);
        return o;
      }

      half4 fragment fragmentColored(VertexOutput in [[stage_in]]) {
        return half4(in.color, 1.0);
      }
    )";

    NS::Error* error = nullptr;
    const auto library = _device->newLibrary(NS::String::string(shaderSourceCode, NS::StringEncoding::UTF8StringEncoding), nullptr, &error);

    if (!library) {
      throw std::runtime_error(error->localizedDescription()->utf8String());
    }

    const auto vertexFunction = library->newFunction(NS::String::string("vertexColored", NS::StringEncoding::UTF8StringEncoding));
    const auto fragmentFunction = library->newFunction(NS::String::string("fragmentColored", NS::StringEncoding::UTF8StringEncoding));

    const auto renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    renderPipelineDescriptor->setVertexFunction(vertexFunction);
    renderPipelineDescriptor->setFragmentFunction(fragmentFunction);
    renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

    _pipelineState = _device->newRenderPipelineState(renderPipelineDescriptor, &error);

    if (!_pipelineState) {
      throw std::runtime_error(error->localizedDescription()->utf8String());
    }

    vertexFunction->release();
    fragmentFunction->release();
    renderPipelineDescriptor->release();
    library->release();
}

void linguine::render::MetalRendererImpl::draw() {
  auto pool = NS::AutoreleasePool::alloc()->init();

  auto commandBuffer = _commandQueue->commandBuffer();

  auto renderCommandEncoder = commandBuffer->renderCommandEncoder(_view.currentRenderPassDescriptor());

  renderCommandEncoder->setRenderPipelineState(_pipelineState);
  renderCommandEncoder->setVertexBuffer(_vertexPositionsBuffer, 0, 0);
  renderCommandEncoder->setVertexBuffer(_vertexColorsBuffer, 0, 1);
  renderCommandEncoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));

  renderCommandEncoder->endEncoding();

  commandBuffer->presentDrawable(_view.currentDrawable());
  commandBuffer->commit();

  pool->release();
}

linguine::render::MetalRenderer* linguine::render::MetalRenderer::create(MTK::View& view) {
  return new MetalRendererImpl(view);
}
