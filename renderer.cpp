#include "renderer.h"
#include "imgui.h"
// #include <__msvc_chrono.hpp>
#include <chrono>
#include <ctime>
#include <glm/geometric.hpp>
#include <memory>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED

// const std::chrono::milliseconds
//     _TARGET_FPS(static_cast<long long>(16.67)); // Target frame time for 60
//     FPS

void renderer::renderLoop() {

  while (!glfwWindowShouldClose(window)) {

    glfwPollEvents();

    // handle physics

    // handle input
    appInput();

    draw_imgui();
    draw();

    vkDeviceWaitIdle(device);
  }
}
void renderer::draw_imgui() {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // 1. Show the big demo window (Most of the sample code is in
  // ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
  // ImGui!).
  // if (show_demo_window)
  //   ImGui::ShowDemoWindow(&show_demo_window);

  // 2. Show a simple window that we create ourselves. We use a Begin/End pair
  // to create a named window.
  {
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Objects edit options");
    objectQueue->flushGuiCalls();
    ImGui::End();

    ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!"
                                   // and append into it.

    ImGui::Text("This is some useful text."); // Display some text (you can
                                              // use a format strings too)

    ImGui::SliderFloat("sides", &lookat0, 0.0f,
                       10.0f); // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::SliderFloat("up to down", &lookat1, 0.0f,
                       10.0f); // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::SliderFloat("depth", &lookat2, 0.0f,
                       10.0f); // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3(
        "clear color",
        (float *)&clear_color); // Edit 3 floats representing a color

    if (ImGui::Button("Button")) // Buttons return true when clicked (most
                                 // widgets return true when edited/activated)
      counter++;

    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / io.Framerate, io.Framerate);
    // ImGui::Text("Cursor Xpos: %.2f and Ypos: %.2f ",
    // static_cast<float>(currX), static_cast<float>(currY));
    ImVec2 mPos = ImGui::GetMousePos();
    ImGui::Text("Cursor Xpos: %.2f and Ypos: %.2f ", mPos.x, mPos.y);
    ImGui::Text("Was Clicked: %i, Last initial click at x: %.2f and y: %.2f",
                LeftClick, initialX, initialY);
    ImGui::Spacing();
    ImGui::Text("Current click at x: %.2f and y: %.2f", currClickX, currClickY);
    ImGui::Text("click Direction: (%.2f , %.2f), click magnitude: %.2f",
                click[0], click[1], clickLength);
    ImGui::Text("clickratio: %.2f", clickRatio);
    ImGui::Text("velocity x: %.2f, y: %.2f",
                objectQueue->shapes.front()->properties->vvelocity[0],
                objectQueue->shapes.front()->properties->vvelocity[1]);
    ImGui::Text("last circle speed: %.2f",
                objectQueue->shapes.front()->properties->fspeed);
    ImGui::End();
  }

  ImGui::Render();
  drawData = ImGui::GetDrawData();
  const bool is_minimized =
      (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f);
  if (!is_minimized) {
    ColorClearValue.color.float32[0] = clear_color.x * clear_color.w;
    ColorClearValue.color.float32[1] = clear_color.y * clear_color.w;
    ColorClearValue.color.float32[2] = clear_color.z * clear_color.w;
    ColorClearValue.color.float32[3] = clear_color.w;
  }
}

void renderer::draw() {
  // Beginning of imgui
  vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE,
                  UINT64_MAX);

  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(
      device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame],
      VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failded to acquire swap chain image!");
  }

  // updateUniformBuffer(currentFrame);

  vkResetFences(device, 1, &inFlightFences[currentFrame]);

  vkResetCommandBuffer(commandBuffers[currentFrame],
                       /*VkCommandBufferResetFlagBits*/ 0);

  recordCommandBuffer(commandBuffers[currentFrame], imageIndex, drawData);
  // this should have to pass {}vertex buffer

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

  VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(graphicsQueue, 1, &submitInfo,
                    inFlightFences[currentFrame]) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {swapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;

  presentInfo.pImageIndices = &imageIndex;

  result = vkQueuePresentKHR(graphicsQueue, &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      framebufferResized) {
    framebufferResized = false;
    recreateSwapChain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failded to present swap chain image!");
  }

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
};

void renderer::recordCommandBuffer(VkCommandBuffer commandBuffer,
                                   uint32_t imageIndex, ImDrawData *draw_data) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = renderPass;
  renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = swapChainExtent;

  // VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  //
  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = ColorClearValue.color;
  clearValues[1].depthStencil = {1.0f, 0};

  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  // renderPassInfo.clearValueCount = 1;
  // renderPassInfo.pClearValues = &ClearValue;

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  // vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
  //                   graphicsPipeline);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)swapChainExtent.width;
  viewport.height = (float)swapChainExtent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = swapChainExtent;
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  objectQueue->flush(commandBuffer);
  // a->injectMethods2commandB(commandBuffer);
  // VkBuffer vertexBuffers[] = {a->vertexBuffer};
  // VkDeviceSize offsets[] = {0};
  // vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
  //
  // vkCmdBindIndexBuffer(commandBuffer, a->indexBuffer, 0,
  // VK_INDEX_TYPE_UINT16);
  //
  // vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
  //                         pipelineLayout, 0, 1,
  //                         &descriptorSets[currentFrame], 0, nullptr);
  //
  // vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(a->indexCount), 1, 0,
  // 0,
  //                  0);

  ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer);

  vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}
void renderer::appInput() {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  if (drawCircle == true) {
    std::unique_ptr<renderobject> appinput =
        std::make_unique<renderobject>(this);
    appinput->mesh = std::make_unique<Scircle>(3.f);
    appinput->prepareRenderProperties();

    // objectQueue->push_renderobject(std::make_shared<renderobject>(this));
    // objectQueue->shapes.back()->mesh = std::make_unique<Scircle>(3.0f);
    // objectQueue->shapes.back()->prepareRenderProperties();
    // set object pos with click pos;
    //
    glm::vec2 norm = {currClickX / width * 2 - 1, currClickY / height * 2 - 1};
    std::cout << "NDC x:" << norm[0] << "and NDC y:" << norm[1] << std::endl;

    glm::mat4 view = glm::lookAt(glm::vec3(lookat0, lookat1, lookat2),
                                 glm::vec3(0.0f), glm::vec3(0, 1.0f, 0));
    glm::mat4 proj = glm::perspective(
        glm::radians(90.0f), getswExtent().width / (float)getswExtent().height,
        0.1f, 100.f);
    proj[1][1] *= -1;

    norm = norm * 10.f;

    appinput->properties->vpos =
        glm::inverse(proj) * glm::inverse(view) * glm::vec4(norm, 0.999f, 10.f);
    appinput->properties->vpos[2] = 0.f;
    objectQueue->push_renderobject(std::move(appinput));

    drawCircle = false;
  }
  if (setVelocityCircle == true) {
    // glm
    int hip = glm::length(glm::vec2(width, height));
    clickRatio = std::min(clickLength / hip, 1.f);
    click = glm::normalize(click) * clickRatio * MAX_VELOCITY;
    click[1] *= -1.f;
    objectQueue->shapes.front()->properties->vvelocity = glm::vec3(click, 0.f);
    objectQueue->shapes.front()->properties->fspeed = clickRatio * MAX_VELOCITY;
    setVelocityCircle = false;
    // objectQueue->shapes.front()->setvelocity();
  }
}
