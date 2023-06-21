cmake_minimum_required(VERSION 3.5)

# set(CMAKE_TOOLCHAIN_FILE C:/Users/aldol/scoop/apps/vcpkg/current/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/build/conan_toolchain.cmake)
# set(CMAKE_TOOLCHAIN_FILE D:/Projects/vulkan/VKtest/build/conan_toolchain.cmake)

# set(CMAKE_BUILD_TYPE R)
# Set the project name
project(circleSim)

add_executable(circleSim main.cpp)

find_package(Vulkan REQUIRED)
# include_directories(${Vulkan_INCLUDE_DIRS})
# link_directories(${Vulkan_LIBRARY_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})

target_include_directories(${PROJECT_NAME} PRIVATE ./dependencies/glfw/include/)
target_link_libraries(${PROJECT_NAME} PRIVATE ./dependencies/glfw/build/src/)

set(glm_DIR ./dependencies/glm/cmake/glm)
find_package(glm REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
# target_include_directories(${PROJECT_NAME} PRIVATE glm)
