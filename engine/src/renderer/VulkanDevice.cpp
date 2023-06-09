#include "VulkanDevice.h"

VulkanDevice::VulkanDevice(const VulkanPhysicalDevice& mPhysicalDevice, const std::vector<const char*>& deviceExtensions):
	VulkanObject{VK_NULL_HANDLE}, mPhysicalDevice(mPhysicalDevice)
{
	// TODO: get queue index from physical device
	float queuePriorities[] = {1.0f};
	uint32_t queueFamilyIndex = 0;

	VkDeviceQueueCreateInfo deviceQueueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
	deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
	deviceQueueCreateInfo.queueCount = 1;
	deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

	VkDeviceCreateInfo deviceCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
	deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	
	VK_CHECK(vkCreateDevice(mPhysicalDevice.getHandle(), &deviceCreateInfo, nullptr, &mHandle));

	volkLoadDevice(mHandle);
}

VulkanDevice::~VulkanDevice()
{
	vkDestroyDevice(mHandle, nullptr);
}
