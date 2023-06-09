#pragma once

#include <vector>

#include "VulkanObject.h"
#include "VulkanPhysicalDevice.h"

class VulkanDevice : public VulkanObject<VkDevice>
{
public:
	VulkanDevice(const VulkanPhysicalDevice& physicalDevice, const std::vector<const char*>& deviceExtensions);

	VulkanDevice(const VulkanDevice& other) = delete;
	VulkanDevice& operator=(const VulkanDevice& other) = delete;
	
	~VulkanDevice() override;

private:
	const VulkanPhysicalDevice& mPhysicalDevice;
	
};
