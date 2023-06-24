cmake_minimum_required(VERSION 3.2)

# set(CMAKE_TOOLCHAIN_FILE C:/Users/aldol/scoop/apps/vcpkg/current/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/build/conan_toolchain.cmake)
# set(CMAKE_TOOLCHAIN_FILE D:/Projects/vulkan/VKtest/build/conan_toolchain.cmake)

# set(CMAKE_BUILD_TYPE R)
# Set the project name
project(circlesim)

set(IMGUI_DIR ./dependencies/dear-imgui/)
# add_executable(circlesim main.cpp )
add_executable(circlesim main.cpp vulkanApp.h utils.h vulkanApp.cpp utils.cpp ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp )
# file(GLOB SOURCES vulkanApp.h utils.h guiManager.h vulkanApp.cpp utils.cpp guiManager.cpp  )
# target_sources(circlesim PRIVATE ${SOURCES} )
# add_executable(circlesim main.cpp ${IMGUI_DIR}/*.cpp ${IMGUI_DIR}/dependencies/*.cpp )

find_package(Vulkan REQUIRED)
# include_directories(${Vulkan_INCLUDE_DIRS})
# link_directories(${Vulkan_LIBRARY_DIRS})
target_include_directories(circlesim PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(circlesim PRIVATE ${Vulkan_LIBRARIES})

target_include_directories(circlesim PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/glfw/include/)
target_link_directories(circlesim PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/glfw/build/src/)
target_link_libraries(circlesim PRIVATE glfw3)

set(glm_DIR ./dependencies/glm/cmake/glm)
find_package(glm REQUIRED)
target_link_libraries(circlesim PRIVATE glm::glm)
# target_include_directories(${PROJECT_NAME} PRIVATE glm)
# include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)

target_include_directories(circlesim PRIVATE ${IMGUI_DIR} ${IMGUI_DIR}/backends)

