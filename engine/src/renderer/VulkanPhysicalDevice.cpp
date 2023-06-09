#include "VulkanPhysicalDevice.h"

VulkanPhysicalDevice::VulkanPhysicalDevice(VkPhysicalDevice physicalDevice):
	VulkanObject{physicalDevice}
{
	vkGetPhysicalDeviceProperties(physicalDevice, &mProperties);
	LOG(INFO, "\tGPU Name: %s", mProperties.deviceName);
	
	uint32_t queueFamilyPropertiesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, nullptr);
	std::vector<VkQueueFamilyProperties>queueFamilyProperties(queueFamilyPropertiesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties.data());
	
	LOG(INFO, "VulkanPhysicalDevice constructed at %p, handle=$lld\n", this, mHandle);
}

const VkPhysicalDeviceProperties& VulkanPhysicalDevice::getProperties() const
{
	return mProperties;
}
