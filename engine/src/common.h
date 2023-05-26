#include <stdio.h>

#define LOG(LOG_LEVEL, ...)                                 \
    do {                                                    \
        printf("Proto[%s]: %s\n", #LOG_LEVEL, __VA_ARGS__); \
    } while(0)

#define VK_CHECK(call)                      \
    do {                                    \
        VkResult result = call;             \
        if (result != VK_SUCCESS)           \
        {                                   \
            LOG(ERROR, #call "failed!");    \
            abort();                        \
        }                                   \
    } while(0)
