#pragma once
#include "renderobjects.h"
#include <algorithm>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <stdexcept>

class physicsEngine {
public :
  bool checkCollisions(renderobject* boundary, renderobject* simObject);
  void updatesimulation(renderObjectQueue* queue);
  bool checkwallcollisions(glm::vec3 p1, glm::vec3 p2, glm::vec3 q1, glm::vec3 q2);
  uint32_t FrameCounter;
};
