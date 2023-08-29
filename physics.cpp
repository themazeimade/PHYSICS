#include "physics.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>

void physicsEngine::checkObjectsCollisions(renderObjectQueue *queue) {
  int frontier = queue->frontier;
  // std::deque<glm::vec3> displacement;
  for (size_t i = 0; i <= frontier; i++) {
    for (size_t j = i + 1; j <= frontier; j++) {
      // if (i == j)
      //   continue;
      auto object01 = queue->shapes[i]->mesh->properties.get();
      auto object02 = queue->shapes[j]->mesh->properties.get();

      float r = 0.0f;
      ;                  // sum of objects radiuses;
      glm::vec3 d(0.0f); // distance from center points of both obj;
      float s = 0.0f;    // difference between d and r
      glm::vec3 n(0.0f);
      glm::vec3 vr01(0.0f);
      glm::vec3 vr02(0.0f);
      float vrn01 = 0.0f;
      float vrn02 = 0.0f;
      glm::vec3 Fi(0.0f);
      float J = 0.0f;

      r = object01->fRadius + object02->fRadius;
      d = object01->vpos - object02->vpos;
      s = glm::length(d) - r;
      // std::cout << "new col pass output:" << std::endl
      //           << "s:  " << s << std::endl;

      if (s <= 0.0f) { // there is collision between these two;
        // d = glm::normalize(d);
        n = glm::normalize(d);
        // std::cout << "n components:  " << n.x << " " << n.y << " " << n.z
        //           << std::endl;
        vr01 = object01->vvelocity - object02->vvelocity;
        vr02 = object02->vvelocity - object01->vvelocity;
        vrn01 = glm::dot(vr01, n);
        vrn02 = glm::dot(vr02, -n);
        float vrnplus = glm::abs(vrn01) + glm::abs(vrn02);
        if (vrn01 < 0.0f) {

          J = -1.0f * (glm::dot(vr01, n)) *
              (1 + static_cast<float>(_RESTITUTION)) /
              (1 / object01->fmass + 1 / object02->fmass);
          Fi = n;
          Fi *= J / static_cast<float>(_TIMESTEP);
          object01->vImpactforces += Fi;
          object01->vpos -= (glm::abs(vrn01) / vrnplus) * n * s;
          // displacement.push_front(n * s);
          object01->bObjectCollision = true;
        }
        if (vrn02 < 0.0f) {

          J = -1.0f * (glm::dot(vr02, -n)) *
              (1 + static_cast<float>(_RESTITUTION)) /
              (1 / object01->fmass + 1 / object02->fmass);
          Fi = -n;
          Fi *= J / static_cast<float>(_TIMESTEP);
          object02->vImpactforces += Fi;
          object02->vpos -= (glm::abs(vrn02) / vrnplus) * -n * s;
          // displacement.push_front(n * s);
          object02->bObjectCollision = true;
        }
        object01 = nullptr;
        object02 = nullptr;
      }
    }
  }
  // int i = 0;
  // for (auto it = displacement.begin(); it <= (displacement.begin() +
  // frontier);
  //      it++) {
  //   queue->shapes[i]->mesh->properties->vpos -= *it;
  //   i++;
  // }
}
bool physicsEngine::checkWallCollisions(renderobject *boundary,
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
  // objectData->vImpactforces = glm::vec3(0.0f, 0.0f, 0.0f);
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
      // std::cout << "there is collision" << std::endl;
      collisionCount++;
      impactDir += glm::normalize(wallPoint01 - wallPoint02);
      if (collisionCount == 1) {
        line::pIntersection(objectData->vpos, objectData->vvelocity,
                            wallPoint01, wallPoint02, intersectionpoint);
      }
    }
    if (collisionCount == 2) {
      impactDir = glm::normalize(impactDir);
      break;
    }
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
    objectData->fNormalForce = dot(objectData->vforces, impactDir);
    objectData->vpos.y = intersectionpoint.y;
    objectData->vpos.x = intersectionpoint.x;
    // if (intersectionpoint.x == 0.0f) {
    // std::cout << "fuck no intersection point" << std::endl;
    // }

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
    checkObjectsCollisions(queue);
    // wall collisions below
    for (auto it = queue->shapes.begin() + _frontier;
         it >= queue->shapes.begin(); it--) {
      auto _simObject = it->get();

      if (_simObject->physicsEnable == false)
        continue;
      _simObject->mesh->properties->bCollision =
          checkWallCollisions(boundingBox, _simObject);
      _simObject->mesh->properties->CalcF();
      _simObject->mesh->properties->updateEuler(dt);
      _simObject = nullptr;
    }

    // checkObjectsCollisions(queue);
    // std::cout << "finished object coll" << std::endl;

    // for (auto it = queue->shapes.begin() + _frontier;
    //      it >= queue->shapes.begin(); it--) {
    //   auto _simObject = it->get();
    //
    //   if (_simObject->physicsEnable == false)
    //     continue;
    //
    //   _simObject->mesh->properties->CalcF();
    //   _simObject->mesh->properties->updateEuler(dt);
    //   _simObject->mesh->properties->vImpactforces = {0.0f,0.0f,0.0f};
    //   _simObject = nullptr;
    // }
  }
  boundingBox = nullptr;
  queue = nullptr;
}
