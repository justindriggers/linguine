#include "TriangleFeatureRenderer.h"

#include <glm/vec2.hpp>

#include "renderer/features/TriangleFeature.h"

namespace linguine::render {

TriangleFeatureRenderer::TriangleFeatureRenderer(MetalRenderContext& context)
    : _context(context) {
  glm::vec2 positions[] = {
      { -0.5f, -0.5f },
      {  0.0f,  0.5f },
      {  0.5f, -0.5f }
  };

  const auto positionsBufferSize = std::size(positions) * sizeof(glm::vec2);
  _vertexPositionsBuffer = context.device->newBuffer(positionsBufferSize, MTL::ResourceStorageModeShared);

  memcpy(_vertexPositionsBuffer->contents(), positions, positionsBufferSize);
  _vertexPositionsBuffer->didModifyRange(NS::Range::Make(0, positionsBufferSize));

  const auto shaderSourceCode = R"(
      struct VertexOutput {
        float4 position [[position]];
        half3 color;
      };

      VertexOutput vertex vertexColored(uint index [[vertex_id]],
          const device float2* positions [[buffer(0)]]) {
        VertexOutput o;
        o.position = float4(positions[index], 0.0, 1.0);
        o.color = half3(1.0, 1.0, 1.0);
        return o;
      }

      half4 fragment fragmentColored(VertexOutput in [[stage_in]]) {
        return half4(in.color, 1.0);
      }
    )";

  NS::Error* error = nullptr;
  const auto library = context.device->newLibrary(NS::String::string(shaderSourceCode, NS::StringEncoding::UTF8StringEncoding), nullptr, &error);

  if (!library) {
    throw std::runtime_error(error->localizedDescription()->utf8String());
  }

  const auto vertexFunction = library->newFunction(NS::String::string("vertexColored", NS::StringEncoding::UTF8StringEncoding));
  const auto fragmentFunction = library->newFunction(NS::String::string("fragmentColored", NS::StringEncoding::UTF8StringEncoding));

  const auto renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
  renderPipelineDescriptor->setVertexFunction(vertexFunction);
  renderPipelineDescriptor->setFragmentFunction(fragmentFunction);
  renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

  _pipelineState = context.device->newRenderPipelineState(renderPipelineDescriptor, &error);

  if (!_pipelineState) {
    throw std::runtime_error(error->localizedDescription()->utf8String());
  }

  vertexFunction->release();
  fragmentFunction->release();
  renderPipelineDescriptor->release();
  library->release();
}

TriangleFeatureRenderer::~TriangleFeatureRenderer() {
  _vertexPositionsBuffer->release();
  _pipelineState->release();
}

bool TriangleFeatureRenderer::isRelevant(Renderable& renderable) {
  return renderable.hasFeature<TriangleFeature>();
}

void TriangleFeatureRenderer::draw() {
  _context.renderCommandEncoder->setRenderPipelineState(_pipelineState);
  _context.renderCommandEncoder->setVertexBuffer(_vertexPositionsBuffer, 0, 0);

  for (const auto& renderable : getRenderables()) {
    if (renderable && renderable->isEnabled()) {
      auto feature = renderable->getFeature<TriangleFeature>();
      _context.renderCommandEncoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));
    }
  }
}

}  // namespace linguine::render
