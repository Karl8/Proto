file(GLOB_RECURSE PROTO_SOURCES "src/*.h" "src/*.cpp")
message(STATUS "PROTO_SOURCES = ${PROTO_SOURCES}")

add_executable(proto ${PROTO_SOURCES})

target_include_directories(proto PRIVATE "src/")

add_subdirectory(extern)

target_link_libraries(proto
    PRIVATE
        # imgui
        glfw
        volk
        # VulkanMemoryAllocator
)