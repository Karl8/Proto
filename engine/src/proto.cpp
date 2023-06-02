#include <iostream>

#include "common.h"

#include "renderer/vulkan_renderer.h"
#include "editor/editor.h"

int main()
{
    LOG(INFO, "Hello Proto!\n");
    
    Renderer *renderer = new VulkanRenderer();
    renderer->exec();

    // Editor protoEditor;
    // protoEditor.start();
    return 0;
}