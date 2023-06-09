#pragma once

#include <vector>
#include <volk.h>

#include "common.h"
#include "VulkanObject.h"

class VulkanPhysicalDevice;

class VulkanInstance : public VulkanObject<VkInstance>
{
public:
	VulkanInstance(const char* applicationName, const std::vector<const char*>& instanceExtentions,
	               const std::vector<const char*>& enabledLayers);
	
	VulkanInstance(VulkanInstance&& other) = delete;
	VulkanInstance& operator=(VulkanInstance&& other) = delete;
	
	~VulkanInstance() override;

	const std::vector<VulkanPhysicalDevice> getPhysicalDevices() const;

	const VulkanPhysicalDevice& pickSuitablePhysicalDevice() const;

private:
	// VkInstance mHandle{VK_NULL_HANDLE};
	std::vector<VulkanPhysicalDevice> mPhysicalDevices;
};
