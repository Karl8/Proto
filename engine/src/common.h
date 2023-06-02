#pragma once
#ifdef _WIN32
    #include <Windows.h>
    #define outputToDebugWindow(x) OutputDebugString(x);
#else
    #define outputToDebugWindow(x)
#endif

#include <stdio.h>
#include <stdexcept>

#define LOG(LOG_LEVEL, str, ...)                                 \
    do {                                                    \
        char log_text_buffer[255];                          \
        sprintf(log_text_buffer, "Proto[" #LOG_LEVEL "]: " str, __VA_ARGS__); \
        fprintf(stdout, "Proto[" #LOG_LEVEL "]: " str, __VA_ARGS__); \
        outputToDebugWindow(log_text_buffer); \
    } while(0)

#define VK_CHECK(call)                      \
    do {                                    \
        VkResult result = call;             \
        if (result != VK_SUCCESS)           \
        {                                   \
            LOG(ERROR, #call "failed!");    \
            throw std::runtime_error(#call "failed!");     \
        }                                   \
    } while(0)
