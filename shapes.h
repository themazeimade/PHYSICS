#include "vulkanApp.h"
#include <glm/fwd.hpp>
#include <memory>
#include <stdint.h>
#include <tuple>
#include <vector>

struct objProperties {
  objProperties();
  // physicsProperties();
  // ~physicsProperties();
  float fmass;
  glm::vec3 vInitialForce;
  glm::vec3 vpos;
  glm::vec3 vvelocity;
  float fspeed;
  glm::vec3 vforces;
  float fRadius;
  glm::vec3 vgravity;
  glm::vec3 vprevPos;
  glm::vec3 vImpactforces;
  glm::vec3 vTangent;
  float fNormalForce;
  bool bCollision;
  bool bObjectCollision;
  bool goingOut;
  
  static bool bounceEnable;


  void CalcF();
  void updateEuler(double dt);
};

class Shape {
public:
  Shape(){
      // buildBuffers();
    properties = std::make_unique<objProperties>();
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
  std::unique_ptr<objProperties> properties;
// protected:
  // void buildBuffers();
  // std::vector<Vertex> getVertices();
  // void destroyBuffers();
};

