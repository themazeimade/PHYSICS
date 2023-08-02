#include "imgui.h"
#include "renderer.h"
#include <iostream>
#include <memory>
// #include "physicsProperties.h"

// double renderer::currX;
// double renderer::currY

int main() {
  try {
    auto app = std::make_unique<renderer>();
    // setup only mesh
    std::unique_ptr<renderobject> b = std::make_unique<renderobject>(app.get());
    b->mesh = std::make_unique<Square>(20.f);
    b->prepareRenderProperties();
    app->objectQueue->push_renderobject(std::move(b));
    // b.reset();
    
    // objQueue->push_renderobject(std::make_unique<renderobject>(app.get()));
    // objQueue->shapes.back()->mesh = new circleFan(5.f, 32);
    // objQueue->shapes.back()->prepareRenderProperties();
    // std::unique_ptr<renderobject> a = std::make_unique<renderobject>(app.get());
    // a->mesh = std::make_unique<Scircle>(3.f);
    // a->prepareRenderProperties();
    // objQueue->push_renderobject(a);
    // a.reset();
    app->renderLoop();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "bye!!" << std::endl;
  return EXIT_SUCCESS;
}
