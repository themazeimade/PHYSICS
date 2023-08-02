#include "vulkanApp.h"
#include <glm/fwd.hpp>
#include <stdint.h>
#include <tuple>
#include <vector>

class Shape {
public:
  Shape(){
      // buildBuffers();
  };
  virtual ~Shape(){
      // destroyBuffers();
  };
  // virtual void render(VkCommandBuffer commandBuffer);
  // std::vector<uint16_t> getIndices();
  // vkEngine* context;
  // VkBuffer vertexBuffer;
  // VkDeviceMemory vertexBufferMemory;
  ShapeType type;
  glm::vec3 center_pos = {0.f, 0.f, 0.f};
  // VkBuffer indexBuffer;
  // VkDeviceMemory indexBufferMemory;
  uint32_t indexCount;
  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;
  bool shaderPrimitive = false;
  bool transparent = false;
// protected:
  // void buildBuffers();
  // std::vector<Vertex> getVertices();
  // void destroyBuffers();
};
