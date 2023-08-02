#include "shapes.h"
#include <memory>

class Square : public Shape {
public:
  Square(float size_) : size(size_) {
    vertices = {{{-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
                {{1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
                {{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
                {{-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}};
    indices = {0, 1, 2, 2, 3, 0};
    for (auto &v : vertices) {
      v.pos = {(size / 2) * v.pos[0], (size / 2) * v.pos[1], 0.0f};
    }
    // buildBuffers();
  }
  ~Square(){};

private:
  float size;
};

class Scircle : public Square {
public:
  Scircle(float radius) : Square(radius) {
 shaderPrimitive = true; 
    transparent = true;
  }
};

class circleFan : public Shape {
public:
  circleFan(float radius_, uint32_t nSegments)
      : radius(radius_), Segments(nSegments) {
    vertices.push_back(
        {{0.0f, 0.0f,0.0f}, {1.0f, 1.0f, 1.0f}}); // Center vertex with white color

    // Generate the vertices for the circle fan
    for (int i = 0; i <= nSegments; ++i) {
      float angle = static_cast<float>(i) / static_cast<float>(nSegments) *
                    2.0f * 3.14159f;
      float x = radius_ * std::cos(angle);
      float y = radius_ * std::sin(angle);
      vertices.push_back({{x, y, 0.0f}, {1.0f, 0.0f, 0.0f}}); // Vertex with red color
                                                        // indices
      indices.push_back(0);     // Index of the center vertex
      indices.push_back(i + 1); // Index of the current vertex
      indices.push_back(i + 2); // Index of the next vertex
    };
    indexCount = indices.size();
    // buildBuffers();
  };
  ~circleFan(){};

private:
  float radius;
  uint32_t Segments;
};
