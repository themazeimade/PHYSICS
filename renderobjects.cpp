#include "renderobjects.h"
#include "imgui.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <stdint.h>

objProperties::objProperties() {
  // std::cout << "properties constructor called" << std::endl;
  fmass = 1.0f;

  vpos = glm::vec3(0.0f, 0.0f, 0.0f);
  vprevPos = glm::vec3(0.0f, 0.0f, 0.0f);

  vvelocity = glm::vec3(0.0f, 0.0f, 0.0f);
  fspeed = 0.0f;
  vforces = glm::vec3(0.0f, 0.0f, 0.0f);
  fNormalForce = 0.0f;
  // fRadius = _radius;
  vgravity.x = 0.0f;
  vgravity.y = fmass * _GRAVITY;
  vImpactforces = glm::vec3(0.0f, 0.0f, 0.0f);
  vTangent = glm::vec3(0.0f);
};

void objProperties::CalcF() {
  vforces = glm::vec3(0.f);

  if (bCollision) {
    // handle friction
    int HoV = 1;
    if (vTangent.x == 1)
      HoV = 0;
    float dirFriction = glm::dot(vforces, vTangent);

    if (glm::abs(dirFriction) <= fNormalForce * _U_STATIC) {
      vforces[HoV] = 0;
    } else {
      glm::vec3 vfriction = vTangent;
      if (dirFriction <= 0) {
        vfriction *= fNormalForce * static_cast<float>(_U_KINETIC);
      } else {
        vfriction *= -1 * fNormalForce * static_cast<float>(_U_KINETIC);
      }
      vforces += vfriction;
    }
    vforces += vImpactforces;
  } else {
    // std::cout << "adding external forces" << std::endl;
    vforces += vgravity;

    glm::vec3 vDrag(0.0f, 0.0f, 0.0f);
    float fDrag(0.0f);

    vDrag -= vvelocity;
    glm::normalize(vDrag);

    fDrag = static_cast<float>(
        0.5 * _AIRDENSITY * static_cast<double>(fspeed * fspeed) *
        static_cast<double>(3.14159f * fRadius * fRadius) * _DRAG);
    vDrag *= fDrag;
    vforces += vDrag;

    // // Wind
    glm::vec3 vWind(0.0f, 0.0f, 0.0f);
    vWind.x = static_cast<float>(
        0.5 * _AIRDENSITY * _WINDSPEED * _WINDSPEED *
        static_cast<double>(3.14159f * fRadius * fRadius) * _DRAG);
    vforces += vWind;
  }
}

void objProperties::updateEuler(double dt) {
  glm::vec3 a;
  glm::vec3 dv;
  glm::vec3 ds(0.0f);

  a = vforces / fmass;
  dv = a * static_cast<float>(dt);
  vvelocity += dv;
  fspeed = glm::length(vvelocity);

  if (fspeed >= 0.5) {
    ds = vvelocity * static_cast<float>(dt);
  };
  // vprevPos = vpos;
  vpos += ds;
  // std::cout << "vprevPos = ("<< vprevPos.x << ";" << vprevPos.y << ")" << std::endl;
  // std::cout << "vpos = ("<< vpos.x << ";" << vpos.y << ")" << std::endl;
  
}

void renderobject::createMeshBuffers() {
  if (mesh->vertices.empty() || mesh->indices.empty()) {
    throw std::runtime_error("no data to be attached to buffer");
  }
  context->createVertexBuffer(vertexBuffer, vertexBufferMemory, mesh->vertices);
  context->createIndexBuffer(indexBuffer, indexBufferMemory, mesh->indices);
  // context->mainDeletion.push_function([=]() { destroyMeshBuffers(); });
}

void renderobject::destroyMeshBuffers() {
  vkDestroyBuffer(context->getDevice(), indexBuffer, nullptr);
  vkFreeMemory(context->getDevice(), indexBufferMemory, nullptr);

  vkDestroyBuffer(context->getDevice(), vertexBuffer, nullptr);
  vkFreeMemory(context->getDevice(), vertexBufferMemory, nullptr);
}

void renderobject::createMVPBuffer() {
  VkDeviceSize bufferSize = sizeof(UniformBufferObject);

  uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
  uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
  uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    context->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                          uniformBuffers[i], uniformBuffersMemory[i]);

    vkMapMemory(context->getDevice(), uniformBuffersMemory[i], 0, bufferSize, 0,
                &uniformBuffersMapped[i]);
  }
  // context->mainDeletion.push_function([=]() {
  //   for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
  //     vkDestroyBuffer(context->getDevice(), uniformBuffers[i], nullptr);
  //     vkFreeMemory(context->getDevice(), uniformBuffersMemory[i], nullptr);
  //   }
  // });
}

void renderobject::buildDescriptorSets() {

  std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT,
                                             context->getDLayout());
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
  allocInfo.pSetLayouts = layouts.data();

  descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
  if (vkAllocateDescriptorSets(context->getDevice(), &allocInfo,
                               descriptorSets.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = uniformBuffers[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSets[i];
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(context->getDevice(), 1, &descriptorWrite, 0,
                           nullptr);
  }
}

void renderobject::createDescriptorPool() {
  VkDescriptorPoolSize poolSize{};
  poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = 1;
  poolInfo.pPoolSizes = &poolSize;
  poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

  if (vkCreateDescriptorPool(context->getDevice(), &poolInfo, nullptr,
                             &descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }

  // context->mainDeletion.push_function([=]() {
  //   vkDestroyDescriptorPool(context->getDevice(), descriptorPool, nullptr);
  // });
}

void renderobject::prepareRenderProperties() {
  createMeshBuffers();
  createMVPBuffer();
  createDescriptorPool();
  buildDescriptorSets();
  createMeshPipeline();
}
void renderobject::injectMethods2commandB(VkCommandBuffer commandbuffer_) {
  updateUBO();
  if (mesh->shaderPrimitive != true) {
    vkCmdBindPipeline(commandbuffer_, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      pipeline);
  } else {
    vkCmdBindPipeline(commandbuffer_, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      *(context->getdefaultPipeline()));
  }
  VkBuffer vertexBuffers[] = {vertexBuffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandbuffer_, 0, 1, vertexBuffers, offsets);
  vkCmdBindIndexBuffer(commandbuffer_, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
  vkCmdBindDescriptorSets(commandbuffer_, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          PipeLayout, 0, 1,
                          &descriptorSets[context->getCurrFrame()], 0, nullptr);
  vkCmdDrawIndexed(commandbuffer_, static_cast<uint32_t>(mesh->indices.size()),
                   1, 0, 0, 0);
}
void renderobject::updateUBO() {
  UniformBufferObject ubo_;
  // do stuff with that
  ubo_.model = glm::translate(ubo_.model, mesh->properties->vpos);
  // ubo_.model = glm::translate(MVP.model,glm::vec3(3.f));
  ubo_.view = glm::lookAt(
      glm::vec3(context->lookat0, context->lookat1, context->lookat2),
      glm::vec3(0.0f), glm::vec3(0, 1.0f, 0));
  ubo_.proj = glm::perspective(glm::radians(90.0f),
                               context->getswExtent().width /
                                   (float)context->getswExtent().height,
                               0.1f, 100.f);
  ubo_.proj[1][1] *= -1;
  memcpy(uniformBuffersMapped[context->getCurrFrame()], &ubo_, sizeof(ubo_));
}

void renderobject::createMeshPipeline() {
  if (mesh->shaderPrimitive != true) {
    context->createDerivativePipeline("../shaders/vert01.spv",
                                      "../shaders/frag01.spv", pipeline);
  }
}

void renderObjectQueue::push_renderobject(std::unique_ptr<renderobject> ro_) {
  if (ro_->mesh->transparent == true) {
    shapes.push_front(std::move(ro_));
    // std::cout << "pushed to the front" << std::endl;
    frontier++;
  } else {
    shapes.push_back(std::move(ro_));
    // std::cout << "pushed to the back" << std::endl;
  }
};

void renderObjectQueue::flush(VkCommandBuffer commandBuffer_) {
  for (auto it = shapes.rbegin(); it != shapes.rend(); it++) {
    (*it)->injectMethods2commandB(commandBuffer_);
  }
}

void renderObjectQueue::flushGuiCalls() {
  int j = 0;
  for (auto it = shapes.rbegin() + 1; it != shapes.rend(); it++) {
    ImGui::PushID(j);
    ImGui::Text("Object %i", j);
    ImGui::Text("Enter a value x:");
    ImGui::SameLine(); // Place the input box on the same line as the label
    ImGui::SliderFloat("x", &((*it)->mesh->properties->vpos.x), -10.0f, 10.0f);
    ImGui::Text("Enter a value y");
    ImGui::SameLine(); // Place the input box on the same line as the label
    ImGui::SliderFloat("y", &((*it)->mesh->properties->vpos.y), -10.0f, 10.0f);
    ImGui::Text("Enter a value z:");
    ImGui::SameLine(); // Place the input box on the same line as the label
    ImGui::SliderFloat("z", &((*it)->mesh->properties->vpos.z), -10.0f, 10.0f);
    ImGui::Spacing();
    ImGui::Text("vvelocity.x: %.3f",
                static_cast<float>((*it)->mesh->properties->vvelocity.x));
    ImGui::Text("vvelocity.y: %.3f",
                static_cast<float>((*it)->mesh->properties->vvelocity.y));
    ImGui::Text("vprevPos.x: %.3f",
                static_cast<float>((*it)->mesh->properties->vprevPos.x));
    ImGui::Text("vprevPos.y: %.3f",
                static_cast<float>((*it)->mesh->properties->vprevPos.y));
    ImGui::PopID();
    j++;
  }
};
