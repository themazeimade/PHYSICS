#include "shapes.h"
#include <stdexcept>
#include <tuple>

void Shape::buildBuffers(vkEngine* context) {
  if (vertices.empty() && indices.empty()) {
throw std::runtime_error("no data to be attached to buffer");
  }
 context->createVertexBuffer(vertexBuffer,vertexBufferMemory, vertices);
  context->createIndexBuffer(indexBuffer, indexBufferMemory, indices);
}
