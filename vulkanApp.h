// #include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "utils.h"
#include <iostream>
#include <stdint.h>
#include <vector>
// #include "shapes.h"

class vkEngine {
public:
  FunctionQueue mainDeletion;
  vkEngine() {
    std::cout << "vkEngine constructor called, starting context" << std::endl;
    startContext();
  };
  virtual ~vkEngine() {
    std::cout << "engine desctructor called" << std::endl;
    cleanup();
  };
  void startContext();

  void createVertexBuffer(VkBuffer &vb, VkDeviceMemory &vBMemory,
                          const std::vector<Vertex> verts);
  void createIndexBuffer(VkBuffer &ib, VkDeviceMemory &iBMemory,
                         const std::vector<uint16_t> indexes);
  VkDevice getDevice() { return device; }

  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory);
  VkDescriptorPool getDPool() { return descriptorPool; }
  VkDescriptorSetLayout getDLayout() { return descriptorSetLayout; }
  VkPipeline *getdefaultPipeline() { return &graphicsPipeline; }
  uint32_t getCurrFrame() { return currentFrame; }
  VkPipelineLayout getPipeLayout() { return pipelineLayout; }
  VkExtent2D getswExtent() { return swapChainExtent; }
  void createDerivativePipeline(std::string vertexshaderFile,
                                std::string fragmentshaderFile,
                                VkPipeline &Gpipeline);

  float lookat0 = 0.0f;
  float lookat1 = 0.0f;
  float lookat2 = 10.0f;
  double initialX = -1.;
  double initialY = -1.;
  double currClickX;
  double currClickY;
  glm::vec2 click = {0.0f, 0.0f};
  float clickLength;
  float clickRatio;
  bool drawCircle = false;
  bool setVelocityCircle = false; 
  bool LeftClick = false;

  static int refreshRate;
  static float frameTime;

protected:
  GLFWwindow *window;
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkSurfaceKHR surface;

  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;
  VkQueue graphicsQueue;
  VkQueue presentQueue;

  VkSwapchainKHR swapChain;
  std::vector<VkImage> swapChainImages;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkFramebuffer> swapChainFramebuffers;

  VkRenderPass renderPass;
  VkDescriptorSetLayout descriptorSetLayout;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;
  VkPipelineCache pipelineCache;
  VkGraphicsPipelineCreateInfo base_pipelineInfo;

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  VkPipelineMultisampleStateCreateInfo multisampling{};
  VkPipelineRasterizationStateCreateInfo rasterizer{};
  VkPipelineViewportStateCreateInfo viewportState{};
  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  VkPipelineDepthStencilStateCreateInfo depthStencil{};
  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                               VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState{};
  VkVertexInputBindingDescription bindingDescription =
      Vertex::getBindingDescription();
  std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions =
      Vertex::getAttributeDescriptions();

  VkCommandPool commandPool;

  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;

  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
  std::vector<void *> uniformBuffersMapped;

  VkDescriptorPool descriptorPool;
  std::vector<VkDescriptorSet> descriptorSets;

  std::vector<VkCommandBuffer> commandBuffers;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  uint32_t currentFrame = 0;
  
  //depth buffer
  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;

  // imgui draw data;
  ImDrawData *drawData;

  bool framebufferResized = false;

  // bool show_demo_window = true;
  // bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  VkClearValue ColorClearValue;

  void initWindow();

  void inputHandler();
  static void framebufferResizeCallback(GLFWwindow *window, int width,
                                        int height);
  // input handlig before imgui;
  static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
  static void mouse_button_callback(GLFWwindow *window, int button, int action,
                                    int mods);
  static void key_callback(GLFWwindow *window, int key, int scancode,
                           int action, int mods);

  void initVulkan();
  // void mainLoop();

  void cleanupSwapChain();
  void cleanup();
  void recreateSwapChain();
  void createInstance();
  void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo);
  void setupDebugMessenger();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createSwapChain();
  void createImageViews();
  void createRenderPass();
  void createDescriptorSetLayout();

  void createGraphicsPipeline();
  void createFramebuffers();

  void createCommandPool();
  void createUniformBuffers();

  void createDescriptorPool();

  void createDescriptorSets();

  void createSyncObjects();

  void updateUniformBuffer(uint32_t currentImage);

  void createDepthResources();
void createImage(uint32_t width, uint32_t height, VkFormat format,
                 VkImageTiling tiling, VkImageUsageFlags usage,
                 VkMemoryPropertyFlags properties, VkImage &image,
                 VkDeviceMemory &imageMemory) ;
  
VkImageView createImageView(VkImage image, VkFormat format,
                            VkImageAspectFlags aspectFlags) ;
  // void drawFrame();

  VkShaderModule createShaderModule(const std::vector<char> &code);

  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);

  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);

  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

  bool isDeviceSuitable(VkPhysicalDevice device);

  bool checkDeviceExtensionSupport(VkPhysicalDevice device);

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

  std::vector<const char *> getRequiredExtensions();
  bool checkValidationLayerSupport();

  static std::vector<char> readFile(const std::string &filename);

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData);

  void init_imgui();
  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
  VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

VkFormat findDepthFormat();
bool hasStencilComponent(VkFormat format);

  void createCommandBuffer();
  // void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t
  // imageIndex,
  //                          ImDrawData *draw_data = nullptr);
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};

