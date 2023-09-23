#include "renderobjects.h"
#include "imgui.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <stdint.h>

bool objProperties::bounceEnable = false;

objProperties::objProperties() {
  // std::cout << "properties constructor called" << std::endl;
  fmass = 1.0f;

  vpos = glm::vec3(0.0f, 0.0f, 0.0f);
  vprevPos = glm::vec3(0.0f, 0.0f, 0.0f);
  vInitialForce = glm::vec3(0.0f);
  vvelocity = glm::vec3(0.0f, 0.0f, 0.0f);
  fspeed = 0.0f;
  vforces = glm::vec3(0.0f, 0.0f, 0.0f);
  fNormalForce = 0.0f;
  fRadius = 1.0f;
  vgravity.x = 0.0f;
  vgravity.y = fmass * static_cast<float>(_GRAVITY);
  vgravity.z = 0.0f;
  vImpactforces = glm::vec3(0.0f, 0.0f, 0.0f);
  vTangent = glm::vec3(0.0f);
  bObjectCollision = false;
  bCollision = false;
  goingOut = false;
};

void objProperties::CalcF() {
  glm::vec3 vforcesImpostor = vforces;
  vforces = glm::vec3(0.f);

  // if(bObjectCollision) {
  //
  // }
  if (bCollision == true || bObjectCollision == true) {
    int HoV = 1;
    if (vTangent.x == 1 || vTangent.x == -1)
      HoV = 0;
    float dirFriction = glm::dot(
        vforcesImpostor, vTangent); // normal force on windd and gravity that
    float dirVelocity =
        glm::dot(vvelocity, vTangent); // normal force on windd and gravity that
    if (bCollision == true) {
      // handle friction
      if (vvelocity[HoV] <= 0.01f && vvelocity[HoV] >= -0.01f) {
        // std::cout << "hi im inside before htis" << std::endl;
        if (glm::abs(dirFriction) <= glm::abs(fNormalForce) * _U_STATIC) {
          // std::cout << "hi im inside this shitt" << std::endl;
          vforces[HoV] = 0.0f;
          vvelocity[HoV] = 0.0f;
        }
      } else {
        glm::vec3 vfriction = vTangent;
        if (dirVelocity < 0.0f) {
          // std::cout << "hi im going to the left" << std::endl;
          vfriction *= -1 * fNormalForce * static_cast<float>(_U_KINETIC);
        } else {
          // std::cout << "hi im goind to the rigth" << std::endl;
          vfriction *= fNormalForce * static_cast<float>(_U_KINETIC);
        }
        vforces += vfriction;
      }
      fNormalForce = -1;
    }
    vforces += vImpactforces;

    // if (bCollision == true) {
    //   if (vvelocity[HoV] <= 0.2f) {
    //     if (glm::abs(dirFriction) <= fNormalForce * _U_STATIC) {
    //       vforces[HoV] = 0.0f;
    //       vvelocity[HoV] = 0.0f;
    //     }
    //   }
    // }

  } else {
    // std::cout << "adding external forces" << std::endl;
    vforces += vgravity;
    vforces += vInitialForce;

    // glm::normalize(vDrag);
    if(objProperties::bounceEnable == false) {
    glm::vec3 vDrag(0.0f, 0.0f, 0.0f);
    float fDrag(0.0f);

    vDrag -= vvelocity;
    if (glm::length(vvelocity) != 0) {
      vDrag = glm::normalize(vDrag);
    }
    
    fDrag = 0.5f * static_cast<float>(_AIRDENSITY) * (fspeed * fspeed) *
            (3.14159f * fRadius * fRadius) * static_cast<float>(_DRAG);
    vDrag *= fDrag;

    vforces += vDrag;
  }

    // std::cout << "vDrag x " <<vDrag.x << std::endl;
    // std::cout << "vDrag y " <<vDrag.y << std::endl;
    // std::cout << "vDrag z " <<vDrag.z << std::endl;
    // // // Wind
    glm::vec3 vWind(0.0f, 0.0f, 0.0f);
    vWind.x = static_cast<float>(
        0.5 * _AIRDENSITY * _WINDSPEED * _WINDSPEED *
        static_cast<double>(3.14159f * fRadius * fRadius) * _DRAG);
    vforces += vWind;
  }

  // std::cout << "forces x: " << vforces.x << std::endl;
  // std::cout << "forces y: " << vforces.y << std::endl;
  bObjectCollision = false;
  // vInitialForce = glm::vec3(0.0f);
}

void objProperties::updateEuler(double dt) {
  glm::vec3 a(0.0f);
  glm::vec3 dv(0.0f);
  glm::vec3 ds(0.0f);

  a = vforces / fmass;
  // std::cout << "vforces x: " << vforces.x << "y: " << vforces.y << std::endl;
  dv = a * static_cast<float>(dt);
  vvelocity += dv;
  // std::cout << "vvelocity x: " << vvelocity.x << "y: " << vvelocity.y <<
  // std::endl;
  fspeed = glm::length(vvelocity);
  // if (fspeed < 0.06f) vvelocity = {0.0f,0.0f,0.0f};
  ds = vvelocity * static_cast<float>(dt);
  // if (bCollision == false)
  // vprevPos = vpos;

  if (glm::length(ds) >= 0.001) {
    vpos += ds;
  }

  vImpactforces = {0.0f, 0.0f, 0.0f};
  vInitialForce = glm::vec3(0.0f);
  // };
  // std::cout << "vprevPos = ("<< vprevPos.x << ";" << vprevPos.y << ")" <<
  // std::endl; std::cout << "vpos = ("<< vpos.x << ";" << vpos.y << ")" <<
  // std::endl;
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

bool renderobject::changedPos() {
  if (mesh->properties->vpos == lastcheckPos) {
    return false;
  }
  lastcheckPos = mesh->properties->vpos;
  return true;
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

#ifdef _MSC_VER
#define DERIV_VERTEXSHADER "../../shaders/vert01.spv"
#define DERIV_FRAGSHADER "../../shaders/frag01.spv"
#endif // DEBUG
//
#ifdef __MINGW32__ 
#define DERIV_VERTEXSHADER "../shaders/vert01.spv"
#define DERIV_FRAGSHADER "../shaders/frag01.spv"
#endif

void renderobject::createMeshPipeline() {
  if (mesh->shaderPrimitive != true) {
    context->createDerivativePipeline(DERIV_VERTEXSHADER,
                                      DERIV_FRAGSHADER, pipeline);
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
    ImGui::Text("vforce.x: %.3f",
                static_cast<float>((*it)->mesh->properties->vforces.x));
    ImGui::Text("vforce.y: %.3f",
                static_cast<float>((*it)->mesh->properties->vforces.y));
    // ImGui::Text("vprevPos.x: %.3f",
    //             static_cast<float>((*it)->mesh->properties->vprevPos.x));
    // ImGui::Text("vprevPos.y: %.3f",
    //             static_cast<float>((*it)->mesh->properties->vprevPos.y));
    ImGui::PopID();
    j++;
  }
};
