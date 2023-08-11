#include <GLFW/glfw3.h>
#include <array>
#include <deque>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <optional>
#include <stdint.h>
#include <vector>
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#define GLM_FORCE_RADIANS
#include <chrono>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <set>
#include <stdexcept>
#include <thread>
#include <iomanip>

#define _GRAVITY -9.8
#define _AIRDENSITY 1.23
#define _DRAG 0.6
#define _WINDSPEED 1.5
#define _TIMESTEP 0.005555 
#define _RESTITUTION 0.5
#define _STEPCOUNT 3
// #define _TARGET_FPS 0.01667

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator);

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete();
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct FunctionQueue // extracted from vkguide  chapter 2
{
  std::deque<std::function<void()>> deletors;

  void push_function(std::function<void()> &&function) {
    deletors.push_back(function);
  }

  void flush() {
    // reverse iterate the deletion queue to execute all the functions
    for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
      (*it)(); // call the function
    }

    deletors.clear();
  }
};

struct Vertex {
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 texcoord;

  static VkVertexInputBindingDescription getBindingDescription();

  static std::array<VkVertexInputAttributeDescription, 3>
  getAttributeDescriptions();
};

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

enum ShapeType {
  vTriangle = 0,
  vQuad = 1,
  vCircle = 2,
};

struct UniformBufferObject {
  glm::mat4 model = glm::mat4(1);
  glm::mat4 view = glm::mat4(1);
  glm::mat4 proj = glm::mat4(1);
};

namespace line {
bool insideSegment(glm::vec3 p, glm::vec3 p1, glm::vec3 p3);
int orientation(glm::vec3 p, glm::vec3 p1, glm::vec3 p3);
bool pIntersection(glm::vec2 p1, glm::vec2 p2, glm::vec2 q1, glm::vec2 q2,
                   glm::vec2 &fill);
} // namespace line
