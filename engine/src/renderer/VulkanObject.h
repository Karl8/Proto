#pragma once

#include <algorithm>
#include <cassert>
#include "vkcommon.h"

static size_t objectCount = 0;	

class VulkanDevice;

template <typename THandle>
class VulkanObject
{
	static const size_t INVALID_ID = -1;
public:
	VulkanObject(THandle mHandle = VK_NULL_HANDLE/*, VulkanDevice* mDevice = nullptr*/) :
		mHandle{mHandle}/*, mDevice{mDevice}*/, mID{generateNewObjectId()}
	{
	}

	VulkanObject(const VulkanObject&) = delete;
	VulkanObject& operator=(const VulkanObject&) = delete;

	VulkanObject(VulkanObject &&other) noexcept : mHandle{std::move(other.mHandle)} /*, mDevice{std::move(other.mDevice)}*/, mID{std::move(other.mID)}
	{
		// other.mHandle = VK_NULL_HANDLE;
		// other.mDevice = nullptr;
		// other.mID = INVALID_ID;
	}
	VulkanObject& operator=(VulkanObject &&other) noexcept
	{
		mHandle = std::move(other.mHandle);
		// mDevice = std::move(other.mDevice);
		mID = std::move(other.mID);

		// other.mHandle = VK_NULL_HANDLE;
		// other.mDevice = nullptr;
		// other.mID = INVALID_ID;

		return *this;
	}

	virtual ~VulkanObject() = default;

	inline const THandle& getHandle() const
	{
		return mHandle;
	}

	// inline const VulkanDevice& getDevice() const
	// {
	// 	assert(mDevice);
	// 	return *mDevice;
	// }
	
protected:
	THandle mHandle;
	// VulkanDevice* mDevice;
	size_t mID; // TODO: Generate ID automatically

private:
	// static size_t objectCount;	
	inline static size_t generateNewObjectId()
	{
		return objectCount++;	
	}
};