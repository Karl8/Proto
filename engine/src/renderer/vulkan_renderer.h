#include "renderer.h"

class VulkanRenderer : public Renderer
{
public:
    VulkanRenderer();
    ~VulkanRenderer();

    void initialize() override;
    void renderLoop() override;
    void terminate() override;

};