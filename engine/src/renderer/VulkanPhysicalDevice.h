#pragma once

#include <vector>

#include "VulkanInstance.h"
#include "VulkanObject.h"

class VulkanPhysicalDevice: public VulkanObject<VkPhysicalDevice>
{
public:
	VulkanPhysicalDevice(VkPhysicalDevice physicalDevice);
	
	const VkPhysicalDeviceProperties& getProperties() const;

private:
	VkPhysicalDeviceProperties mProperties{};

	// const VulkanInstance& instance;
	
};
