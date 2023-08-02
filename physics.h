#include "physicsProperties.h"

class physicsEngine {
public :
  bool checkCollisions(renderobject* boundary, renderobject* simObject);
  void updatesimulation();
  bool checkwallcollisions(glm::vec3 p1, glm::vec3 p2, glm::vec3 q1, glm::vec3 q2);
  uint32_t FrameCounter;
};
