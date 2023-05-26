#include <volk.h>
#include <vector>

#include "common.h"
#include "vulkan_renderer.h"

#include <assert.h>

const char* instance_extensions[2] = {
	VK_KHR_SURFACE_EXTENSION_NAME,
	VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
	VK_USE_PLATFORM_WIN32_KHR
	"VK_KHR_win32_surface"
};


VulkanRenderer::VulkanRenderer()
{
}

VulkanRenderer::~VulkanRenderer()
{
}

void VulkanRenderer::initialize()
{
	VK_CHECK(volkInitialize());

	VkApplicationInfo applicationInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
	applicationInfo.apiVersion = VK_VERSION_1_0;

	const std::vector<const char*> instanceExtentions = {
		VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef VK_USE_PLATFORM_WIN32_KHR
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
	};

	const std::vector<const char*> enabledLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	VkInstanceCreateInfo instanceCreateInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
	instanceCreateInfo.pApplicationInfo = &applicationInfo;
	instanceCreateInfo.enabledExtensionCount = instanceExtentions.size();
	instanceCreateInfo.ppEnabledExtensionNames = instanceExtentions.data();
	instanceCreateInfo.enabledLayerCount = enabledLayers.size();
	instanceCreateInfo.ppEnabledLayerNames = enabledLayers.data();

	VkInstance instance;
	VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

	volkLoadInstanceOnly(instance);

	uint32_t phycicalDeviceCount = 0;
	VK_CHECK(vkEnumeratePhysicalDevices(instance, &phycicalDeviceCount, nullptr));
	assert(phycicalDeviceCount > 0);

	std::vector<VkPhysicalDevice> physicalDevices(phycicalDeviceCount);
	VK_CHECK(vkEnumeratePhysicalDevices(instance, &phycicalDeviceCount, physicalDevices.data()));

	// TODO: pick a suitable device which has graphics queue, present queue, and at least one swapchain format and present mode.
	VkPhysicalDevice physicalDevice = physicalDevices[0];

	const std::vector<const char*> deviceExtensions{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	// uint32_t deviceExtensionCount = 0;
	// vkEnumerateDeviceExtensionProperties(physicalDevice, 
	
	
	
	
	
	// VkInstance instance
	//
	// VkDevice = volkLoadDevice();

}
