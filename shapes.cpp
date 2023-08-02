#include "shapes.h"
#include <stdexcept>
#include <tuple>

// void Shape::buildBuffers() {
//   if (vertices.empty() || indices.empty()) {
// throw std::runtime_error("no data to be attached to buffer");
//   }
//  context->createVertexBuffer(vertexBuffer,vertexBufferMemory, vertices);
//   context->createIndexBuffer(indexBuffer, indexBufferMemory, indices);
// }
// void Shape::destroyBuffers() {
//     vkDestroyBuffer(context->getDevice(), indexBuffer, nullptr);
//   vkFreeMemory(context->getDevice(), indexBufferMemory, nullptr);
//
//   vkDestroyBuffer(context->getDevice(), vertexBuffer, nullptr);
//   vkFreeMemory(context->getDevice(), vertexBufferMemory, nullptr);
//
// };
