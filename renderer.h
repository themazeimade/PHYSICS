// #include "geometry.h"
#pragma once
#include "renderobjects.h"
#include "imgui.h"
#include <memory>
#include <string>
#include "physics.h"

// #include <queue>



class renderer : public vkEngine {
public:
  renderer() : vkEngine() {
    objectQueue = std::make_unique<renderObjectQueue>();
    physics = std::make_unique<physicsEngine>();
  };
  ~renderer(){
    std::cout << "renderer destructor called" << std::endl;
  };
  void renderLoop();

  std::unique_ptr<renderObjectQueue> objectQueue;
  std::unique_ptr<physicsEngine> physics;
private:
  
  void draw();
  void draw_imgui();
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex,
                           ImDrawData *draw_data);
  void appInput();
};
