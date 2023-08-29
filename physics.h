#pragma once
#include "renderobjects.h"
#include <algorithm>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <stdexcept>

class physicsEngine {
public :
  bool checkWallCollisions(renderobject* boundary, renderobject* simObject);
  void checkObjectsCollisions(renderObjectQueue* queue);
  void updatesimulation(renderObjectQueue* queue);
  uint32_t FrameCounter;
};
