// #include "geometry.h"
#include "imgui.h"
#include "physicsProperties.h"
#include <memory>
#include <string>
// #include <queue>

struct renderObjectQueue {
  renderObjectQueue() { /* shapes.clear(); */ };
  ~renderObjectQueue() {

    std::cout << "renderQueue destructor called" << std::endl;
  };

  std::deque<std::unique_ptr<renderobject>> shapes;

  void push_renderobject(std::unique_ptr<renderobject> ro_) {
    if(ro_->mesh->transparent == true) {
    shapes.push_front(std::move(ro_));
      std::cout << "pushed to the front" << std::endl;
    } else {
    shapes.push_back(std::move(ro_));
    }
  };

  void flush(VkCommandBuffer commandBuffer_) {
    for (auto it = shapes.rbegin(); it != shapes.rend(); it++) {
      (*it)->injectMethods2commandB(commandBuffer_);
    }
  }

  void flushGuiCalls() {
    int j = 0;
    for (auto it = shapes.rbegin(); it != shapes.rend(); it++) {
      ImGui::PushID(j);
      ImGui::Text("Object %i", j);
      ImGui::Text("Enter a value x:");
      ImGui::SameLine(); // Place the input box on the same line as the label
      ImGui::SliderFloat("x", &((*it)->properties->vpos.x), 0.0f, 10.0f);
      ImGui::Text("Enter a value y");
      ImGui::SameLine(); // Place the input box on the same line as the label
      ImGui::SliderFloat("y", &((*it)->properties->vpos.y), 0.0f, 10.0f);
      ImGui::Text("Enter a value z:");
      ImGui::SameLine(); // Place the input box on the same line as the label
      ImGui::SliderFloat("z", &((*it)->properties->vpos.z), 0.0f, 10.0f);
      ImGui::Spacing();
      ImGui::PopID();
      j++;
    }
  }
};

class renderer : public vkEngine {
public:
  renderer() : vkEngine() {
    objectQueue = std::make_unique<renderObjectQueue>();
  };
  ~renderer(){
    std::cout << "renderer destructor called" << std::endl;
  };
  void renderLoop();

  std::unique_ptr<renderObjectQueue> objectQueue;
private:
  
  void draw();
  void draw_imgui();
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex,
                           ImDrawData *draw_data);
  void appInput();
};
