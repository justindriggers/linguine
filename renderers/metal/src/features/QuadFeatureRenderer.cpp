#include "QuadFeatureRenderer.h"

#include "renderer/features/QuadFeature.h"

namespace linguine::render {

QuadFeatureRenderer::QuadFeatureRenderer(MetalRenderContext& context)
    : _context(context) {
  simd::float2 positions[] = {
      { -0.5f, -0.5f },
      { -0.5f,  0.5f },
      {  0.5f, -0.5f },

      {  0.5f, -0.5f },
      { -0.5f,  0.5f },
      {  0.5f,  0.5f }
  };

  const auto positionsBufferSize = std::size(positions) * sizeof(simd::float2);
  _vertexPositionsBuffer = context.device->newBuffer(positionsBufferSize, MTL::ResourceStorageModeShared);

  memcpy(_vertexPositionsBuffer->contents(), positions, positionsBufferSize);
  _vertexPositionsBuffer->didModifyRange(NS::Range::Make(0, positionsBufferSize));

  const auto shaderSourceCode = R"(
      struct MetalQuadFeature {
        metal::float4x4 modelMatrix;
        float value;
      };

      struct VertexOutput {
        float4 position [[position]];
        half3 color;
      };

      VertexOutput vertex vertexColored(uint index [[vertex_id]],
          const device float2* positions [[buffer(0)]],
          const constant MetalQuadFeature& feature [[buffer(1)]]) {
        VertexOutput o;
        o.position = feature.modelMatrix * float4(positions[index], 0.0, 1.0);
        o.color = half3(feature.value, 0.0, 0.0);
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

QuadFeatureRenderer::~QuadFeatureRenderer() {
  for (const auto& valueBuffer : _valueBuffers) {
    valueBuffer->release();
  }

  _vertexPositionsBuffer->release();
  _pipelineState->release();
}

bool QuadFeatureRenderer::isRelevant(Renderable& renderable) {
   return renderable.hasFeature<QuadFeature>();
}

void QuadFeatureRenderer::draw() {
  _context.renderCommandEncoder->setRenderPipelineState(_pipelineState);
  _context.renderCommandEncoder->setVertexBuffer(_vertexPositionsBuffer, 0, 0);

  const auto renderables = getRenderables();

  while (_valueBuffers.size() < renderables.size()) {
    _valueBuffers.push_back(_context.device->newBuffer(sizeof(MetalQuadFeature), MTL::ResourceStorageModeShared));
  }

  for (int i = 0; i < renderables.size(); ++i) {
    auto renderable = renderables[i];

    if (renderable && renderable->isEnabled()) {
      auto feature = renderable->getFeature<QuadFeature>();

      auto valueBuffer = _valueBuffers[i];
      auto metalQuadFeature = static_cast<MetalQuadFeature*>(valueBuffer->contents());

      memcpy(&metalQuadFeature->modelMatrix, &feature->modelMatrix, sizeof(simd::float4x4));
      memcpy(&metalQuadFeature->value, &feature->value, sizeof(float));
      valueBuffer->didModifyRange(NS::Range::Make(0, sizeof(MetalQuadFeature)));

      _context.renderCommandEncoder->setVertexBuffer(valueBuffer, 0, 1);
      _context.renderCommandEncoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(6));
    }
  }
}

}  // namespace linguine::render
