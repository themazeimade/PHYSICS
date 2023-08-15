// #include <glm/fwd.hpp>
#pragma once
#include "geometry.h"
#include <iostream>
#include <memory>
#include <vector>

#define MAX_VELOCITY 25.0f

// struct objProperties {
//   // physicsProperties();
//   // ~physicsProperties();
//   float fmass;
//   glm::vec3 vpos;
//   glm::vec3 vvelocity;
//   float fspeed;
//   glm::vec3 vforces;
//   float fRadius;
//   glm::vec3 vgravity;
//   glm::vec3 vprevPos;
//   glm::vec3 vImpactforces;
//   bool bCollision;
//
//   objProperties(/* float _radius */);
//
//   void CalcF();
//   void updateEuler(double dt);
// };

class renderobject {
public:
  renderobject(vkEngine *context_) : context(context_) {
    PipeLayout = context_->getPipeLayout();
    physicsEnable = false;
    // properties = new objProperties;
    // mesh->properties = std::make_unique<objProperties>();
  };
  ~renderobject() { 
    // destroyMeshBuffers(); 
    destroyMeshBuffers();
// destroyUniforBuffers
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      vkDestroyBuffer(context->getDevice(), uniformBuffers[i], nullptr);
      vkFreeMemory(context->getDevice(), uniformBuffersMemory[i], nullptr);
    }
// destroy descriptor pool
    vkDestroyDescriptorPool(context->getDevice(), descriptorPool, nullptr);

    context = nullptr;

  std::cout << "renderobject destructor called" << std::endl;
  // if(mesh->shaderPrimitive != true) {
  //   }
  }

  vkEngine* context;
  std::unique_ptr<Shape> mesh;
  // std::unique_ptr<objProperties> properties;
  UniformBufferObject MVP;

  bool physicsEnable;
  // fill objects
  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
  std::vector<void *> uniformBuffersMapped;

  VkBuffer vertexBuffer;
  VkBuffer indexBuffer;
  VkDeviceMemory vertexBufferMemory;
  VkDeviceMemory indexBufferMemory;
  std::vector<VkDescriptorSet> descriptorSets;
  VkDescriptorPool descriptorPool;
  VkPipeline pipeline;
  VkPipelineLayout PipeLayout;
  // methods
  void createMeshBuffers();
  void createMVPBuffer();
  void updateUBO();
  void buildDescriptorSets();
  void createDescriptorPool();
  void createMeshPipeline();
  void destroyMeshBuffers();
  void destroyDescriptorSets();
  void destroyUniforBuffers();
  void prepareRenderProperties();

  void injectMethods2commandB(VkCommandBuffer commandbuffer_);
};

struct renderObjectQueue {
  renderObjectQueue() { /* shapes.clear(); */ };
  ~renderObjectQueue() {

    std::cout << "renderQueue destructor called" << std::endl;
  };

  std::deque<std::unique_ptr<renderobject>> shapes;
  //opaqye frontier, first transparent index object fron rbegin to rend
  int frontier = -1;

  void push_renderobject(std::unique_ptr<renderobject> ro_) ;

  void flush(VkCommandBuffer commandBuffer_) ;

  void flushGuiCalls();
};
