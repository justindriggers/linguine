#include "ColoredFeatureRenderer.h"

#include "renderer/features/ColoredFeature.h"

namespace linguine::render {

ColoredFeatureRenderer::ColoredFeatureRenderer(MetalRenderContext& context,
                                               Camera& camera,
                                               MeshRegistry& meshRegistry)
    : _context(context), _camera(camera), _meshRegistry(meshRegistry) {
  _cameraBuffer = context.device->newBuffer(sizeof(MetalCamera), MTL::ResourceStorageModeShared);

  const auto shaderSourceCode = R"(
      struct MetalCamera {
        metal::float4x4 viewProjectionMatrix;
      };

      struct MetalColoredFeature {
        metal::float4x4 modelMatrix;
        float3 color;
      };

      struct VertexOutput {
        float4 position [[position]];
        float3 color;
      };

      VertexOutput vertex vertexColored(uint index [[vertex_id]],
          const device float2* positions [[buffer(0)]],
          const constant MetalCamera& camera [[buffer(1)]],
          const constant MetalColoredFeature& feature [[buffer(2)]]) {
        VertexOutput o;
        o.position = camera.viewProjectionMatrix * feature.modelMatrix * float4(positions[index], 0.0, 1.0);
        o.color = feature.color;
        return o;
      }

      float4 fragment fragmentColored(VertexOutput in [[stage_in]]) {
        return float4(in.color, 1.0);
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
  renderPipelineDescriptor->setDepthAttachmentPixelFormat(MTL::PixelFormat::PixelFormatDepth32Float);

  _pipelineState = context.device->newRenderPipelineState(renderPipelineDescriptor, &error);

  if (!_pipelineState) {
    throw std::runtime_error(error->localizedDescription()->utf8String());
  }

  const auto depthStencilDescriptor = MTL::DepthStencilDescriptor::alloc()->init();
  depthStencilDescriptor->setDepthCompareFunction(MTL::CompareFunction::CompareFunctionLessEqual);
  depthStencilDescriptor->setDepthWriteEnabled(true);

  _depthState = context.device->newDepthStencilState(depthStencilDescriptor);

  if (!_depthState) {
    throw std::runtime_error(error->localizedDescription()->utf8String());
  }

  depthStencilDescriptor->release();
  vertexFunction->release();
  fragmentFunction->release();
  renderPipelineDescriptor->release();
  library->release();
}

ColoredFeatureRenderer::~ColoredFeatureRenderer() {
  for (const auto& valueBuffer : _valueBuffers) {
    valueBuffer->release();
  }

  _cameraBuffer->release();
  _pipelineState->release();
  _depthState->release();
}

bool ColoredFeatureRenderer::isRelevant(Renderable& renderable) {
  return renderable.hasFeature<ColoredFeature>();
}

void ColoredFeatureRenderer::draw() {
  auto commandEncoder = _context.commandBuffer->renderCommandEncoder(_context.coloredRenderPassDescriptor);

  commandEncoder->setRenderPipelineState(_pipelineState);
  commandEncoder->setDepthStencilState(_depthState);

  auto metalCamera = static_cast<MetalCamera*>(_cameraBuffer->contents());
  memcpy(&metalCamera->viewProjectionMatrix, &_camera.viewProjectionMatrix, sizeof(simd::float4x4));
  _cameraBuffer->didModifyRange(NS::Range::Make(0, sizeof(MetalCamera)));
  commandEncoder->setVertexBuffer(_cameraBuffer, 0, 1);

  const auto renderables = getRenderables();

  while (_valueBuffers.size() < renderables.size()) {
    _valueBuffers.push_back(_context.device->newBuffer(sizeof(MetalColoredFeature), MTL::ResourceStorageModeShared));
  }

  for (int i = 0; i < renderables.size(); ++i) {
    auto renderable = renderables[i];

    if (renderable && renderable->isEnabled()) {
      auto feature = renderable->getFeature<ColoredFeature>();

      auto& mesh = _meshRegistry.get(feature->meshType);
      mesh->bind(*commandEncoder);

      auto valueBuffer = _valueBuffers[i];
      auto metalColoredFeature = static_cast<MetalColoredFeature*>(valueBuffer->contents());

      memcpy(&metalColoredFeature->modelMatrix, &feature->modelMatrix, sizeof(simd::float4x4));
      memcpy(&metalColoredFeature->color, &feature->color, sizeof(simd::float3));
      valueBuffer->didModifyRange(NS::Range::Make(0, sizeof(MetalColoredFeature)));

      commandEncoder->setVertexBuffer(valueBuffer, 0, 2);
      mesh->draw(*commandEncoder);
    }
  }

  commandEncoder->endEncoding();
}

}  // namespace linguine::render
