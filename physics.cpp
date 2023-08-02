#include "physics.h"
#include <algorithm>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <stdexcept>

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
  glm::vec3 n;
  glm::vec3 vr;
  float vrn;
  float J;
  glm::vec3 Fi;
  bool hasCollision = false;

  std::vector<Vertex> &vertices = boundary->mesh->vertices;
  objProperties &objectData = *simObject->properties;

  //  setup checkCollisions
  if (typeid(*boundary->mesh) != typeid(Square))
    throw std::runtime_error("boundary not Square");

  if (objectData.vpos.x <= vertices[0].pos.x + objectData.fRadius ||
      objectData.vpos.y <= vertices[0].pos.y + objectData.fRadius ||
      objectData.vpos.y >= vertices[2].pos.y - objectData.fRadius ||
      objectData.vpos.x >= vertices[2].pos.x - objectData.fRadius) {
    // check for collision inside boundary
    glm::vec3 collidedWall;
    glm::vec3 intersectionpoint;
    glm::vec3 wallPoint01;
    glm::vec3 wallPoint02;
    for (int i = 0; i <= 3; i++) {
      int before = (i + 3) % 4;
      wallPoint02 = glm::vec3(vertices[before].pos, 0.f);
      wallPoint01 = glm::vec3(vertices[i].pos, 0.f);
      switch (i) {
      case 0:
        wallPoint01.x += objectData.fRadius;
        wallPoint02.x += objectData.fRadius;
      case 1:
        wallPoint01.y += objectData.fRadius;
        wallPoint02.y += objectData.fRadius;
      case 2:
        wallPoint01.x -= objectData.fRadius;
        wallPoint02.x -= objectData.fRadius;
      case 3:
        wallPoint01.y -= objectData.fRadius;
        wallPoint02.y -= objectData.fRadius;
      }
      if (checkwallcollisions(objectData.vpos, objectData.vprevPos,
                              wallPoint01,
                              wallPoint02)) {
        // collidedWall = glm::vec3(vertices[before].pos - vertices[i].pos, 0.f);
        collidedWall = wallPoint01 - wallPoint02; 
        line::pIntersection(objectData.vpos, objectData.vprevPos, wallPoint01 , wallPoint02 ,
                            intersectionpoint);
        break;
      }
    }

    glm::vec3 impactDir = glm::normalize(collidedWall);
    glm::mat4 rotationMat =
        glm::rotate(rotationMat, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
    impactDir = rotationMat * glm::vec4(impactDir, 0.f);
    vr = objectData.vvelocity;
    vrn = glm::dot(vr, impactDir);
    if (vrn < 0.0f) {
      J = -(vrn) * (1 - _RESTITUTION) * objectData.fmass;
      Fi = impactDir;
      Fi *= J / _TIMESTEP;

      objectData.vImpactforces = Fi;
      objectData.vpos.y = intersectionpoint.y;
      objectData.vpos.x = intersectionpoint.x;
      //   objectData.vImpactforces += Fi;
      //   objectData.vpos.y = _GROUND_PLANE + objectData.fRadius;
      //   objectData.vpos.x =
      //       ((_GROUND_PLANE +
      //         objectData.fRadius − objectData.vPreviousPosition.y) /
      //        (objectData.vpos.y - objectData.vprevPos.y) *
      //        (objectData.vpos.x - objectData.vprevPos.x)) +
      //       objectData.vprevPos.x;
      //   hasCollision = true;
    }
  };
    return hasCollision;
};
  // void physicsEngine::updatesimulation() {
    //   for(i=0; i<_MAX_NUM_UNITS; i++)
    // {
    // Units[i].bCollision = CheckForCollisions(&(Units[i]));
    // Units[i].CalcLoads();
    // Units[i].UpdateBodyEuler(dt);
  // }
