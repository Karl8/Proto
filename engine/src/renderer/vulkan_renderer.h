#include "renderer.h"

class VulkanRenderer : Renderer
{
public:
    VulkanRenderer();
    ~VulkanRenderer();

    void initialize() override;

};