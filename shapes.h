#include "vulkanApp.h"
#include <glm/fwd.hpp>
#include <stdint.h>
#include <tuple>
#include <vector>

class Shape {
public:
  Shape();
  ~Shape();
  // virtual void render(VkCommandBuffer commandBuffer);
  void buildBuffers(vkEngine *context);
  // std::vector<Vertex> getVertices();
  // std::vector<uint16_t> getIndices();
protected:
  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;
  uint32_t indexCount;
  ShapeType type;

  glm::vec3 center_pos = {0.f, 0.f, 0.f};
};

class Square : public Shape {
public:
  Square(float size_) : size(size_) {
    for (auto v : vertices) {
    v.pos = { (size_/2)*v.pos[0], (size_/2)*v.pos[1] };
    }
  }

private:
  float size;
  const std::vector<Vertex> vertices = {{{-1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}},
                                        {{1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
                                        {{1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
                                        {{-1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}};
  const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};
};

class circleFan : public Shape {
public:
  circleFan(float radius_, uint32_t nSegments)
      : radius(radius_), Segments(nSegments) {
    vertices.push_back(
        {{0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}); // Center vertex with white color

    // Generate the vertices for the circle fan
    for (int i = 0; i <= nSegments; ++i) {
      float angle = static_cast<float>(i) / static_cast<float>(nSegments) *
                    2.0f * 3.14159f;
      float x = radius_ * std::cos(angle);
      float y = radius_ * std::sin(angle);
      vertices.push_back({{x, y}, {1.0f, 0.0f, 0.0f}}); // Vertex with red color
                                                        // indices
      indices.push_back(0);     // Index of the center vertex
      indices.push_back(i + 1); // Index of the current vertex
      indices.push_back(i + 2); // Index of the next vertex
    };
  }

private:
  float radius;
  uint32_t Segments;
};
