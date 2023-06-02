#include <volk.h>
#include <vector>

#include "common.h"
#include "vulkan_renderer.h"

#include <assert.h>

#include "GLFW/glfw3.h"

#include <vulkan/vk_enum_string_helper.h>

VkInstance instance;

VkPhysicalDevice physicalDevice;
VkDevice device;
VkQueue queue;

GLFWwindow* window = nullptr;
VkSurfaceKHR surface;

const int width = 1280, height = 720;

VkSwapchainKHR swapchain;
VkSurfaceFormatKHR swapchainFormat;
uint32_t imageCount;
std::vector<VkImage> swapchainImages;
std::vector<VkImageView> swapchainImageViews;

VkSemaphore imageAvaliableSemephore;
VkSemaphore renderFinishedSemaphore;

VkFence fence{VK_NULL_HANDLE};

VkRenderPass renderPass;
std::vector<VkFramebuffer> frameBuffers;

VkCommandPool commandPool;

bool isDepthOnlyFormat(VkFormat format)
{
	return format == VK_FORMAT_D16_UNORM ||
		format == VK_FORMAT_D32_SFLOAT;
}

bool isDepthStencilFormat(VkFormat format)
{
	return format == VK_FORMAT_D16_UNORM_S8_UINT ||
		format == VK_FORMAT_D24_UNORM_S8_UINT ||
		format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
		isDepthOnlyFormat(format);
}


VkSurfaceFormatKHR chooseSwapchainFormat()
{
	uint32_t surfaceFormatCount = 0;
	std::vector<VkSurfaceFormatKHR> surfaceFormats;
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr));
	surfaceFormats.resize(surfaceFormatCount);
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats.data()));

	LOG(INFO, "Avaliable swapchain formats:\n");
	for (const auto& surfaceFormat : surfaceFormats)
	{
		LOG(INFO, "\tformat=%s, colorspace=%s\n", string_VkFormat(surfaceFormat.format),
		    string_VkColorSpaceKHR(surfaceFormat.colorSpace));
	}

	// for (const VkSurfaceFormatKHR& surfaceFormat : surfaceFormats)
	// {
	// 	if (surfaceFormat.format == VK_FORMAT_R8G8B8A8_SRGB	|| surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB)
	// 		return surfaceFormat;
	// }

	auto surfaceFormatIt = std::find_if(surfaceFormats.begin(), surfaceFormats.end(),
	                                    [](const VkSurfaceFormatKHR& surfaceFormat)
	                                    {
		                                    return surfaceFormat.format == VK_FORMAT_R8G8B8A8_SRGB || surfaceFormat.
			                                    format == VK_FORMAT_B8G8R8A8_SRGB;
	                                    });
	if (surfaceFormatIt == surfaceFormats.end())
	{
		assert(!surfaceFormats.empty());
		surfaceFormatIt = surfaceFormats.begin();
	}

	LOG(INFO, "Choose swapchain format: format=%s, colorspace=%s\n", string_VkFormat(surfaceFormatIt->format),
	    string_VkColorSpaceKHR(surfaceFormatIt->colorSpace));
	// return *(const_cast<std::vector<VkSurfaceFormatKHR>::iterator&>(surfaceFormatIt));
	return *surfaceFormatIt;
}

VulkanRenderer::VulkanRenderer()
{
}

VulkanRenderer::~VulkanRenderer()
{
}

void VulkanRenderer::initialize()
{
	/* Vulkan Instance */
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

	VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

	volkLoadInstanceOnly(instance);

	/* Physical Device */

	uint32_t phycicalDeviceCount = 0;
	VK_CHECK(vkEnumeratePhysicalDevices(instance, &phycicalDeviceCount, nullptr));
	assert(phycicalDeviceCount > 0);

	std::vector<VkPhysicalDevice> physicalDevices(phycicalDeviceCount);
	VK_CHECK(vkEnumeratePhysicalDevices(instance, &phycicalDeviceCount, physicalDevices.data()));

	// TODO: pick a suitable physical device which:
	//	1. has graphics queue and present queue
	//	2. has at least one swapchain format
	//	3. has at least one present mode.
	physicalDevice = physicalDevices[0];

	/* Logical Device */

	float queuePriorities[] = {1.0f};
	uint32_t queueFamilyIndex = 0;

	VkDeviceQueueCreateInfo deviceQueueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
	deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
	deviceQueueCreateInfo.queueCount = 1;
	deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

	const std::vector<const char*> deviceExtensions{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkDeviceCreateInfo deviceCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
	deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	VK_CHECK(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device));

	volkLoadDevice(device);

	vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);

	/* Window, Surface, Swapchian */

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = glfwCreateWindow(width, height, "proto", 0, 0);
	assert(window);

	VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, &surface));
	// VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);

	// TODO: check swaphchain capabilities

	swapchainFormat = chooseSwapchainFormat();

	VkSwapchainCreateInfoKHR swapchainCreateInfo = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.minImageCount = 2;
	// TODO: check swaphchain image format using vkGetPhysicalDeviceSurfaceFormatsKHR
	swapchainCreateInfo.imageFormat = swapchainFormat.format;
	swapchainCreateInfo.imageColorSpace = swapchainFormat.colorSpace;
	// TODO: use imageExtent from vkGetPhysicalDeviceSurfaceFormatsKHR
	swapchainCreateInfo.imageExtent.width = width;
	swapchainCreateInfo.imageExtent.height = height;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR; // to be optimized
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // to be optimized
	swapchainCreateInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR; // to be optimized

	VK_CHECK(vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain));

	VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr));

	swapchainImages.resize(imageCount);
	VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data()));

	for (auto& swapchainImage : swapchainImages)
	{
		VkImageViewCreateInfo imageViewCreateInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
		imageViewCreateInfo.image = swapchainImage;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = swapchainFormat.format;
		// imageViewCreateInfo.components =
		if (isDepthStencilFormat(swapchainFormat.format))
		{
			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		}
		else
		{
			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageView);
		swapchainImageViews.emplace_back(imageView);
	}

	VkSemaphoreCreateInfo semaphoreCreateInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
	VK_CHECK(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvaliableSemephore));
	VK_CHECK(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphore));

	VkFenceCreateInfo fenceCreateInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
	VK_CHECK(vkCreateFence(device, &fenceCreateInfo, nullptr, &fence));

	/* Renderpass & Framebuffer */
	VkAttachmentDescription attachmentDescription = {};
	attachmentDescription.format = swapchainFormat.format;
	attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference attachmentReference = {};
	attachmentReference.attachment = 0;
	attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	

	VkSubpassDescription subpassDescription = {};
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.inputAttachmentCount = 0;
	subpassDescription.pInputAttachments = nullptr;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &attachmentReference;
	subpassDescription.pResolveAttachments = nullptr;
	subpassDescription.pDepthStencilAttachment = nullptr;
	subpassDescription.preserveAttachmentCount = 0;
	subpassDescription.pPreserveAttachments = nullptr;
	
	VkRenderPassCreateInfo renderPassCreateInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &attachmentDescription;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpassDescription;
	renderPassCreateInfo.dependencyCount = 0;
	renderPassCreateInfo.pDependencies = nullptr;
	
	VK_CHECK(vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass));

	for (auto& swapchainImageView : swapchainImageViews)
	{
		VkFramebufferCreateInfo framebufferCreateInfo = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
		framebufferCreateInfo.renderPass = renderPass;
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = &swapchainImageView;
		framebufferCreateInfo.width = width;
		framebufferCreateInfo.height = height;
		framebufferCreateInfo.layers = 1;

		VkFramebuffer framebuffer;
		VK_CHECK(vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffer));
		frameBuffers.emplace_back(framebuffer);
	}

	/* Command Buffer */
	VkCommandPoolCreateInfo commandPoolCreateInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
	commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

	VK_CHECK(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool));
}


void VulkanRenderer::renderLoop()
{
	while (true)
	{
		/* Aquire next image */
		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvaliableSemephore, VK_NULL_HANDLE,
		                                        &imageIndex);
		LOG(INFO, "imageIndex = %d\n", imageIndex);
		assert(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR);

		/* Draw */
		VkCommandBuffer commandBuffer;
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
		commandBufferAllocateInfo.commandPool = commandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;
		
		vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);

		VkCommandBufferBeginInfo commandBufferBeginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VK_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

		VkClearValue clearValue;
		clearValue.color.float32[0] = 1.0f;
		clearValue.color.float32[1] = 0.0f;
		clearValue.color.float32[2] = 0.0f;
		clearValue.color.float32[3] = 1.0f;
		
		// clearValue.depthStencil = {0.0f, 0};

		VkRenderPassBeginInfo renderPassBeginInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.framebuffer = frameBuffers[imageIndex];
		renderPassBeginInfo.renderArea.offset = {0, 0};
		renderPassBeginInfo.renderArea.extent = {width, height};
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearValue;

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdEndRenderPass(commandBuffer);

		vkEndCommandBuffer(commandBuffer);

		/* Submit */

		VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &imageAvaliableSemephore;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &renderFinishedSemaphore;

		vkResetFences(device, 1, &fence);
		
		vkQueueSubmit(queue, 1, &submitInfo, fence);

		/* Present */
		VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain;
		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(queue, &presentInfo);

		vkDeviceWaitIdle(device);
	}
}

void VulkanRenderer::terminate()
{
	for (auto& imageView : swapchainImageViews)
	{
		vkDestroyImageView(device, imageView, nullptr);
	}
	swapchainImageViews.resize(0);

	for (auto& frameBuffer: frameBuffers)
	{
		vkDestroyFramebuffer(device, frameBuffer, nullptr);
	}
	frameBuffers.resize(0);
	
	vkDestroySwapchainKHR(device, swapchain, nullptr);
	
	vkDestroyRenderPass(device, renderPass, nullptr);

	vkDestroyCommandPool(device, commandPool, nullptr);

	vkDestroySemaphore(device, imageAvaliableSemephore, nullptr);
	vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
	vkDestroyFence(device, fence, nullptr);

	vkDestroyDevice(device, nullptr);

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);
	glfwTerminate();
}
