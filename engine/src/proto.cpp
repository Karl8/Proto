#include <iostream>

#include "common.h"

#include "renderer/vulkan_renderer.h"
#include "editor/editor.h"

int main()
{
    LOG(LOG_INO, "Hello Proto!");
    
    VulkanRenderer renderer;
    renderer.initialize();

    Editor protoEditor;
    protoEditor.start();
    return 0;
}