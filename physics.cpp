#include "physics.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>

bool physicsEngine::checkwallcollisions(glm::vec3 p1, glm::vec3 p2,
                                        glm::vec3 q1, glm::vec3 q2) {
  // retrieve and learned from :
  // https://www.geeksforgeeks.org/orientation-3-ordered-points/
  int o1 = line::orientation(p1, p2, q1);
  int o2 = line::orientation(p1, p2, q2);
  int o3 = line::orientation(q1, q2, p1);
  int o4 = line::orientation(q1, q2, p2);

  if (o1 != o2 && o3 != o4)
    return true;

  // Special Cases
  // p1, q1 and p2 are collinear and p2 lies on segment p1q1
  if (o1 == 0 && line::insideSegment(p1, q1, p2))
    return true;

  // p1, q1 and q2 are collinear and q2 lies on segment p1q1
  if (o2 == 0 && line::insideSegment(p1, q2, p2))
    return true;

  // p2, q2 and p1 are collinear and p1 lies on segment p2q2
  if (o3 == 0 && line::insideSegment(q1, p1, q2))
    return true;

  // p2, q2 and q1 are collinear and q1 lies on segment p2q2
  if (o4 == 0 && line::insideSegment(q1, p2, q2))
    return true;

  return false; // Doesn't fall in any of the above cases
}

bool physicsEngine::checkCollisions(renderobject *boundary,
                                    renderobject *simObject) {
  // std::cout << std::endl << "Physics output" << std::endl;
  // glm::vec3 n(0.0f, 0.0f, 0.0f);
  glm::vec3 vr(0.0f, 0.0f, 0.0f);
  float vrn(0.0f);
  float J(0.0f);
  glm::vec3 Fi(0.0f, 0.0f, 0.0f);
  bool hasCollision = false;

  std::vector<Vertex> wallverts = boundary->mesh->vertices;
  objProperties *objectData = simObject->mesh->properties.get();
  objectData->vImpactforces = glm::vec3(0.0f, 0.0f, 0.0f);
  // glm::vec3 storePrevPos(0.0f);
  // storePrevPos = objectData->vpos;

  Shape *check = boundary->mesh.get();

  //  setup checkCollisions
  if (typeid(*(check)) != typeid(Square))
    // if (typeid(*(boundary->mesh.get())) != typeid(Square))
    throw std::runtime_error("boundary not Square");
  check = nullptr;

  // if (objectData->vpos.x <= wallverts[0].pos.x + objectData->fRadius ||
  //     objectData->vpos.y <= wallverts[0].pos.y + objectData->fRadius ||
  //     objectData->vpos.y >= wallverts[2].pos.y - objectData->fRadius ||
  //     objectData->vpos.x >= wallverts[2].pos.x - objectData->fRadius) {
  // left vertical wall, down wall, up wall, right wall;

  // check for collision inside boundary
  glm::vec3 collidedWall(0.0f);
  glm::vec2 intersectionpoint(0.0f);
  glm::vec3 impactDir(0.0f);
  glm::mat4 rotationMat =
      glm::rotate(glm::identity<glm::mat4>(), glm::radians(90.0f),
                  glm::vec3(0.0f, 0.0f, 1.0f));
  int collisionCount = 0;
  bool wallColl = false;

  for (int i = 0; i <= 3; i++) {
    glm::vec3 wallPoint02(0.0f);
    glm::vec3 wallPoint01(0.0f);
    int before = (i + 3) % 4;
    wallPoint02 = wallverts[before].pos;
    wallPoint01 = wallverts[i].pos;
    int wall = -1;
    switch (i) {
    case 0:
      wallPoint01.x += objectData->fRadius;
      wallPoint02.x += objectData->fRadius;
      wallColl = objectData->vpos.x <= wallverts[0].pos.x + objectData->fRadius;
      wall = 0;
      break;
    case 1:
      wallPoint01.y += objectData->fRadius;
      wallPoint02.y += objectData->fRadius;
      wallColl = objectData->vpos.y <= wallverts[0].pos.y + objectData->fRadius;
      wall = 1;
      break;
    case 2:
      wallPoint01.x -= objectData->fRadius;
      wallPoint02.x -= objectData->fRadius;
      wallColl = objectData->vpos.x >= wallverts[2].pos.x - objectData->fRadius;
      wall = 2;
      break;
    case 3:
      wallPoint01.y -= objectData->fRadius;
      wallPoint02.y -= objectData->fRadius;
      wallColl = objectData->vpos.y >= wallverts[2].pos.y - objectData->fRadius;
      wall = 3;
      break;
    }
    if (wallColl == true) {

      // collidedWall = wallPoint01 - wallPoint02;
      // std::cout << "collision on wall: " << wall << std::endl;
      // std::cout << objectData->vpos.x << " pos.x" << std::endl;
      // std::cout << objectData->vpos.y << " pos.y" << std::endl;
      // std::cout << objectData->vforces.x << " forces.x" << std::endl;
      // std::cout << objectData->vforces.y << " forces.y" << std::endl;
      collisionCount++;
      impactDir += glm::normalize(wallPoint01 - wallPoint02);
      if (collisionCount == 1) {
        line::pIntersection(objectData->vpos, objectData->vvelocity,
                            wallPoint01, wallPoint02, intersectionpoint);
      }
    }
    if (collisionCount == 2)
      break;
  }
  objectData->vTangent = impactDir;
  impactDir = rotationMat * glm::vec4(impactDir, 0.f);
  vr = objectData->vvelocity;
  vrn = glm::dot(vr, impactDir);

  if (vrn < 0.0f) {
    // std::cout << "inside impulse handling" << std::endl;
    J = -1.0f * (glm::dot(vr, impactDir)) *
        (1 + static_cast<float>(_RESTITUTION)) * objectData->fmass;
    Fi = impactDir;
    Fi *= J / static_cast<float>(_TIMESTEP);
    objectData->vImpactforces = Fi;
    objectData->fNormalForce = glm::length(Fi);
    // if (intersectionpoint.x == 0.0f) {
    // std::cout << "fuck no intersection point" << std::endl;
    // }
    objectData->vpos.y = intersectionpoint.y;
    objectData->vpos.x = intersectionpoint.x;

    hasCollision = true;
  }
  // };
  // objectData->vprevPos = storePrevPos;
  objectData = nullptr;
  return hasCollision;
};
void physicsEngine::updatesimulation(renderObjectQueue *queue) {
  int &_frontier = queue->frontier;
  if (_frontier == -1) {
    std::cout << "frontier not iniated" << std::endl;
    return;
  }
  double dt = _TIMESTEP;
  auto boundingBox = queue->shapes.back().get();

  for (int i = 0; i < _STEPCOUNT; i++) {
    for (auto it = queue->shapes.begin() + _frontier;
         it >= queue->shapes.begin(); it--) {
      auto _simObject = it->get();
      if (_simObject->physicsEnable == false) continue;
      _simObject->mesh->properties->bCollision =
          checkCollisions(boundingBox, _simObject);
      _simObject->mesh->properties->CalcF();
      _simObject->mesh->properties->updateEuler(dt);
      _simObject = nullptr;
    }
  }
  boundingBox = nullptr;
  queue = nullptr;
}
