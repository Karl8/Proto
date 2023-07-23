#include "VulkanDevice.h"

VulkanDevice::VulkanDevice(const VulkanPhysicalDevice& physicalDevice, const std::vector<const char*>& deviceExtensions):
	VulkanObject{VK_NULL_HANDLE}, mPhysicalDevice(physicalDevice)
{
	// TODO: get queue index from physical device
	float queuePriorities[] = {1.0f};
	uint32_t queueFamilyIndex = 0;

	VkDeviceQueueCreateInfo deviceQueueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
	deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
	deviceQueueCreateInfo.queueCount = 1;
	deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

#ifdef VK_USE_PLATFORM_MACOS_MVK
    VkPhysicalDevicePortabilitySubsetFeaturesKHR portabilityFeatures = {};
    portabilityFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2 = {};
    physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    physicalDeviceFeatures2.pNext = &portabilityFeatures;
    vkGetPhysicalDeviceFeatures2(mPhysicalDevice.getHandle(), &physicalDeviceFeatures2);
#endif

	VkDeviceCreateInfo deviceCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
	deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
#ifdef VK_USE_PLATFORM_MACOS_MVK
    deviceCreateInfo.pNext = (VkPhysicalDevicePortabilitySubsetFeaturesKHR*)&portabilityFeatures;
#endif
	
	VK_CHECK(vkCreateDevice(mPhysicalDevice.getHandle(), &deviceCreateInfo, nullptr, &mHandle));

	volkLoadDevice(mHandle);
}

VulkanDevice::~VulkanDevice()
{
	vkDestroyDevice(mHandle, nullptr);
}
