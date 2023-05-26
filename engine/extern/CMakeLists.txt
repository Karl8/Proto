# glfw
add_subdirectory(glfw)

# imgui
file(GLOB imgui_SOURCES "imgui/*.h" "imgui/*.cpp")

add_library(imgui ${imgui_SOURCES})

target_include_directories(imgui PUBLIC "imgui/")

# volk
if(WIN32)
  # target_compile_definitions(proto PRIVATE VK_USE_PLATFORM_WIN32_KHR GLFW_EXPOSE_NATIVE_WIN32)
  # target_compile_definitions(proto PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
  set(VOLK_STATIC_DEFINES "VK_USE_PLATFORM_WIN32_KHR")
endif()

add_subdirectory(volk)

# VMA
set(VMA_STATIC_VULKAN_FUNCTIONS OFF)
add_subdirectory(VulkanMemoryAllocator)