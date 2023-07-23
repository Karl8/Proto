#pragma once

#include <memory>

#include "renderer.h"

class VulkanInstance;
class VulkanDevice;

class VulkanRenderer : public Renderer
{
public:
    VulkanRenderer();
    ~VulkanRenderer() override;

    void initialize() override;
    void renderLoop() override;
    void terminate() override;

private:
    void initialize_legacy();
    void renderLoop_legacy();

    std::unique_ptr<VulkanInstance> mInstance;

    std::unique_ptr<VulkanDevice> mDevice;

};
