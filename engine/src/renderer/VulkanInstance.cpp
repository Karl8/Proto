#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"

#include <cassert>

VulkanInstance::VulkanInstance(const char* applicationName, const std::vector<const char*>& instanceExtentions, const std::vector<const char*>& enabledLayers) :
	VulkanObject{VK_NULL_HANDLE}
{
	VK_CHECK(volkInitialize());

	VkApplicationInfo applicationInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
	applicationInfo.pApplicationName = applicationName; 
	applicationInfo.applicationVersion = 0;
	applicationInfo.pEngineName = "Proto";
	applicationInfo.engineVersion = 0;
	applicationInfo.apiVersion = VK_VERSION_1_0;

	// TODO: check extensions available
	VkInstanceCreateInfo instanceCreateInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
	instanceCreateInfo.pApplicationInfo = &applicationInfo;
	instanceCreateInfo.enabledExtensionCount = instanceExtentions.size();
	instanceCreateInfo.ppEnabledExtensionNames = instanceExtentions.data();
	instanceCreateInfo.enabledLayerCount = enabledLayers.size();
	instanceCreateInfo.ppEnabledLayerNames = enabledLayers.data();

	VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &mHandle));
	
	volkLoadInstanceOnly(mHandle);

	mPhysicalDevices = getPhysicalDevices();

	LOG(INFO, "VulkanInstance constructed at %p, mHandle=%lld.\n", this, mHandle);
}

VulkanInstance::~VulkanInstance()
{
	vkDestroyInstance(mHandle, nullptr);
}

const std::vector<VulkanPhysicalDevice> VulkanInstance::getPhysicalDevices() const
{
	uint32_t phycicalDeviceCount = 0;
	VK_CHECK(vkEnumeratePhysicalDevices(mHandle, &phycicalDeviceCount, nullptr));
	assert(phycicalDeviceCount > 0);

	std::vector<VkPhysicalDevice> physicalDeviceHandles(phycicalDeviceCount);
	VK_CHECK(vkEnumeratePhysicalDevices(mHandle, &phycicalDeviceCount, physicalDeviceHandles.data()));

	std::vector<VulkanPhysicalDevice> gpus;
	for (auto& physicalDeviceHandle: physicalDeviceHandles)
	{
		// VulkanPhysicalDevice physicalDevice(physicalDeviceHandle);
		// mPhysicalDevices.emplace_back(std::move(physicalDevice));
		// mPhysicalDevices.emplace_back(std::move(VulkanPhysicalDevice(physicalDeviceHandle)));
		// mPhysicalDevices.emplace_back(std::make_unique<VulkanPhysicalDevice>(physicalDeviceHandle));
		gpus.emplace_back(physicalDeviceHandle);
	}
	
	return gpus;
}

const VulkanPhysicalDevice& VulkanInstance::pickSuitablePhysicalDevice() const
{
	for(const auto& physicalDevice: mPhysicalDevices)
	{
		const auto& properties = physicalDevice.getProperties();
		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			return physicalDevice;
	}

	return mPhysicalDevices[0];
}



