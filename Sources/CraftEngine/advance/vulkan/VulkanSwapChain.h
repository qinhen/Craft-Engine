#pragma once
#ifndef VULKAN_SWAPCHAIN_H_
#define VULKAN_SWAPCHAIN_H_

#include <stdlib.h>
#include <string>
#include <assert.h>
#include <stdio.h>
#include <vector>
#include <stdexcept>
#include <iostream>

#pragma comment( lib, "vulkan-1.lib" ) 


#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include<vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>


#define VULKAN_SWAPCHAIN_CHECK_RESULT(f)																\
{																										\
	VkResult res = (f);																					\
	if (res != VK_SUCCESS)																				\
	{																									\
		std::cout << "Fatal : VkResult is \"" << res << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl; \
		assert(res == VK_SUCCESS);																		\
	}																									\
}                                                                                                       \

namespace CraftEngine
{

	namespace vulkan
	{

		class SwapChain
		{
		private:


			VkInstance       m_instance = VK_NULL_HANDLE;
			VkDevice         m_logicalDevice = VK_NULL_HANDLE;
			VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

			VkSurfaceKHR    m_surface = VK_NULL_HANDLE;
			VkFormat        m_colorFormat;
			VkColorSpaceKHR m_colorSpace;
			VkSwapchainKHR  m_swapChain = VK_NULL_HANDLE;
			uint32_t        m_imageCount;
			std::vector<VkImage>     m_images;
			std::vector<VkImageView> m_imageViews;


			uint32_t m_queueFamilyIndex;

		public:

			SwapChain()
			{
				m_instance = VK_NULL_HANDLE;
				m_logicalDevice = VK_NULL_HANDLE;
				m_physicalDevice = VK_NULL_HANDLE;
				m_surface = VK_NULL_HANDLE;
				m_swapChain = VK_NULL_HANDLE;
				m_imageCount = 0;
			}


			void connect(VkInstance instance,
				VkPhysicalDevice physicalDevice,
				VkDevice device,
				uint32_t queueFamilyIndex = 0
			)
			{
				this->m_instance = instance;
				this->m_physicalDevice = physicalDevice;
				this->m_logicalDevice = device;
				m_queueFamilyIndex = queueFamilyIndex;
			}

			/** @brief Creates the platform specific surface abstraction of the native platform window used for presentation */
			void initSurface(HINSTANCE hInstance, HWND hWnd);

			VkFormat format() { return m_colorFormat; }
			VkSwapchainKHR swapChain() { return m_swapChain; }
			const std::vector<VkImageView>& imageViews() { return m_imageViews; };
			const std::vector<VkImage>& images() { return m_images; };

			/**
			* Create the swapchain and get it's images with given width and height
			*
			* @param width Pointer to the width of the swapchain (may be adjusted to fit the requirements of the swapchain)
			* @param height Pointer to the height of the swapchain (may be adjusted to fit the requirements of the swapchain)
			* @param vsync (Optional) Can be used to force vsync'd rendering (by using VK_PRESENT_MODE_FIFO_KHR as presentation mode)
			*/
			bool create(uint32_t* width, uint32_t* height, bool vsync = true);

			/**
			* Acquires the next image in the swap chain
			*
			* @param presentCompleteSemaphore (Optional) Semaphore that is signaled when the image is ready for use
			* @param imageIndex Pointer to the image index that will be increased if the next image could be acquired
			*
			* @note The function will always wait until the next image has been acquired by setting timeout to UINT64_MAX
			*
			* @return VkResult of the image acquisition
			*/
			VkResult acquireNextImage(VkSemaphore presentCompleteSemaphore, uint32_t* imageIndex)
			{
				// By setting timeout to UINT64_MAX we will always wait until the next image has been acquired or an actual error is thrown
				// With that we don't have to handle VK_NOT_READY
				return vkAcquireNextImageKHR(m_logicalDevice, m_swapChain, 0, presentCompleteSemaphore, (VkFence)nullptr, imageIndex); // INT64_MAX
			}

			/**
			* Queue an image for presentation
			*
			* @param queue Presentation queue for presenting the image
			* @param imageIndex Index of the swapchain image to queue for presentation
			* @param waitSemaphore (Optional) Semaphore that is waited on before the image is presented (only used if != VK_NULL_HANDLE)
			*
			* @return VkResult of the queue presentation
			*/
			VkResult queuePresent(VkQueue queue, uint32_t imageIndex, uint32_t waitCount, VkSemaphore* pWaitSemaphores);

			/**
			* Destroy and free Vulkan resources used for the swapchain
			*/
			void cleanup();

		};



		void SwapChain::initSurface(HINSTANCE hInstance, HWND hWnd)
		{
			VkResult err = VK_SUCCESS;

			// Create the os-specific surface
			VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
			surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			surfaceCreateInfo.hinstance = hInstance;
			surfaceCreateInfo.hwnd = hWnd;
			err = vkCreateWin32SurfaceKHR(m_instance, &surfaceCreateInfo, nullptr, &m_surface);

			if (err != VK_SUCCESS) {
				throw std::runtime_error("不能创建Surface");
			}

			VkBool32 surfaceSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, m_queueFamilyIndex, m_surface, &surfaceSupport);
			if (surfaceSupport != VK_TRUE) {
				throw std::runtime_error("不能该设备或队列族不支持对应surface");
			}



			// Get list of supported surface formats
			uint32_t formatCount;
			VULKAN_SWAPCHAIN_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &formatCount, NULL));
			assert(formatCount > 0);

			std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
			VULKAN_SWAPCHAIN_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &formatCount, surfaceFormats.data()));

			// If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
			// there is no preferered format, so we assume VK_FORMAT_B8G8R8A8_UNORM
			if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
			{
				m_colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
				m_colorSpace = surfaceFormats[0].colorSpace;
			}
			else
			{
				// iterate over the list of available surface format and
				// check for the presence of VK_FORMAT_B8G8R8A8_UNORM
				bool found_B8G8R8A8_UNORM = false;
				for (auto&& surfaceFormat : surfaceFormats)
				{
					if (surfaceFormat.format == VK_FORMAT_R8G8B8A8_UNORM)
					{
						m_colorFormat = surfaceFormat.format;
						m_colorSpace = surfaceFormat.colorSpace;
						found_B8G8R8A8_UNORM = true;
						break;
					}
				}

				// in case VK_FORMAT_B8G8R8A8_UNORM is not available
				// select the first available color format
				if (!found_B8G8R8A8_UNORM)
				{
					m_colorFormat = surfaceFormats[0].format;
					m_colorSpace = surfaceFormats[0].colorSpace;
				}
			}

		}

		/**
		* Create the swapchain and get it's images with given width and height
		*
		* @param width Pointer to the width of the swapchain (may be adjusted to fit the requirements of the swapchain)
		* @param height Pointer to the height of the swapchain (may be adjusted to fit the requirements of the swapchain)
		* @param vsync (Optional) Can be used to force vsync'd rendering (by using VK_PRESENT_MODE_FIFO_KHR as presentation mode)
		*/

		inline bool SwapChain::create(uint32_t* width, uint32_t* height, bool vsync)
		{
			VkSwapchainKHR oldSwapchain = m_swapChain;
			// Get physical device surface properties and formats
			VkSurfaceCapabilitiesKHR surfCaps;
			VULKAN_SWAPCHAIN_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_surface, &surfCaps));

			// Get available present modes
			uint32_t presentModeCount;
			VULKAN_SWAPCHAIN_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, NULL));
			assert(presentModeCount > 0);

			std::vector<VkPresentModeKHR> presentModes(presentModeCount);
			VULKAN_SWAPCHAIN_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, presentModes.data()));

			VkExtent2D swapchainExtent = {};

			// If width (and height) equals the special value 0xFFFFFFFF, the size of the surface will be set by the swapchain
			if (surfCaps.currentExtent.width == (uint32_t)-1)
			{
				// If the surface size is undefined, the size is set to
				// the size of the images requested.
				swapchainExtent.width = *width;
				swapchainExtent.height = *height;
			}
			else
			{
				// If the surface size is defined, the swap chain size must match
				swapchainExtent = surfCaps.currentExtent;
				*width = surfCaps.currentExtent.width;
				*height = surfCaps.currentExtent.height;
			}
			if (*width == 0 || *height == 0)
				return false;


			// Select a present mode for the swapchain

			// The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
			// This mode waits for the vertical blank ("v-sync")
			VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

			// If v-sync is not requested, try to find a mailbox mode
			// It's the lowest latency non-tearing present mode available
			if (!vsync)
			{
				for (size_t i = 0; i < presentModeCount; i++)
				{
					if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
					{
						swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
						break;
					}
					if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
					{
						swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
					}
				}
			}

			// Determine the number of images
			uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount;//更改
			if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
			{
				desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
			}

			// Find the transformation of the surface
			VkSurfaceTransformFlagsKHR preTransform;
			if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
			{
				// We prefer a non-rotated transform
				preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
			}
			else
			{
				preTransform = surfCaps.currentTransform;
			}

			// Find a supported composite alpha format (not all devices support alpha opaque)
			VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			// Simply select the first composite alpha format available
			std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
				VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
				VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
				VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
				VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
			};
			for (auto& compositeAlphaFlag : compositeAlphaFlags) {
				if (surfCaps.supportedCompositeAlpha & compositeAlphaFlag) {
					compositeAlpha = compositeAlphaFlag;
					break;
				};
			}

			VkSwapchainCreateInfoKHR swapchainCI = {};
			swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapchainCI.pNext = NULL;
			swapchainCI.surface = m_surface;
			swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
			swapchainCI.imageFormat = m_colorFormat;
			swapchainCI.imageColorSpace = m_colorSpace;
			swapchainCI.imageExtent = { swapchainExtent.width, swapchainExtent.height };
			swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			swapchainCI.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
			swapchainCI.imageArrayLayers = 1;
			swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchainCI.queueFamilyIndexCount = 0;
			swapchainCI.pQueueFamilyIndices = NULL;
			swapchainCI.presentMode = swapchainPresentMode;
			swapchainCI.oldSwapchain = oldSwapchain;
			// Setting clipped to VK_TRUE allows the implementation to discard rendering outside of the surface area
			swapchainCI.clipped = VK_TRUE;
			swapchainCI.compositeAlpha = compositeAlpha;

			// Enable transfer source on swap chain images if supported
			if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
				swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			}

			// Enable transfer destination on swap chain images if supported
			if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
				swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			}



			VULKAN_SWAPCHAIN_CHECK_RESULT(vkCreateSwapchainKHR(m_logicalDevice, &swapchainCI, nullptr, &m_swapChain));

			// If an existing swap chain is re-created, destroy the old swap chain
			// This also cleans up all the presentable images
			if (oldSwapchain != VK_NULL_HANDLE)
			{
				for (uint32_t i = 0; i < m_imageViews.size(); i++)
				{
					vkDestroyImageView(m_logicalDevice, m_imageViews[i], nullptr);
				}
				m_imageViews.resize(0);
				vkDestroySwapchainKHR(m_logicalDevice, oldSwapchain, nullptr);
			}
			VULKAN_SWAPCHAIN_CHECK_RESULT(vkGetSwapchainImagesKHR(m_logicalDevice, m_swapChain, &m_imageCount, NULL));

			// Get the swap chain images
			m_images.resize(m_imageCount);
			VULKAN_SWAPCHAIN_CHECK_RESULT(vkGetSwapchainImagesKHR(m_logicalDevice, m_swapChain, &m_imageCount, m_images.data()));

			// Get the swap chain buffers containing the image and imageview
			m_imageViews.resize(m_imageCount);
			for (uint32_t i = 0; i < m_imageCount; i++)
			{
				VkImageViewCreateInfo colorAttachmentView = {};
				colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				colorAttachmentView.pNext = NULL;
				colorAttachmentView.format = m_colorFormat;
				colorAttachmentView.components = {
					VK_COMPONENT_SWIZZLE_R,
					VK_COMPONENT_SWIZZLE_G,
					VK_COMPONENT_SWIZZLE_B,
					VK_COMPONENT_SWIZZLE_A
				};
				colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				colorAttachmentView.subresourceRange.baseMipLevel = 0;
				colorAttachmentView.subresourceRange.levelCount = 1;
				colorAttachmentView.subresourceRange.baseArrayLayer = 0;
				colorAttachmentView.subresourceRange.layerCount = 1;
				colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
				colorAttachmentView.flags = 0;

				colorAttachmentView.image = m_images[i];

				VULKAN_SWAPCHAIN_CHECK_RESULT(vkCreateImageView(m_logicalDevice, &colorAttachmentView, nullptr, &m_imageViews[i]));
			}
			return true;
		}

		/**
		* Queue an image for presentation
		*
		* @param queue Presentation queue for presenting the image
		* @param imageIndex Index of the swapchain image to queue for presentation
		* @param waitSemaphore (Optional) Semaphore that is waited on before the image is presented (only used if != VK_NULL_HANDLE)
		*
		* @return VkResult of the queue presentation
		*/

		inline VkResult SwapChain::queuePresent(VkQueue queue, uint32_t imageIndex, uint32_t waitCount, VkSemaphore* pWaitSemaphores)
		{
			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.pNext = NULL;
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &m_swapChain;
			presentInfo.pImageIndices = &imageIndex;
			// Check if a wait semaphore has been specified to wait for before presenting the image
			if (pWaitSemaphores != VK_NULL_HANDLE)
			{
				presentInfo.pWaitSemaphores = pWaitSemaphores;
				presentInfo.waitSemaphoreCount = waitCount;
			}
			return vkQueuePresentKHR(queue, &presentInfo);
		}

		/**
		* Destroy and free Vulkan resources used for the swapchain
		*/

		inline void SwapChain::cleanup()
		{
			if (m_swapChain != VK_NULL_HANDLE)
			{
				for (uint32_t i = 0; i < m_imageCount; i++)
				{
					vkDestroyImageView(m_logicalDevice, m_imageViews[i], nullptr);
				}
				m_imageViews.resize(0);
			}
			if (m_surface != VK_NULL_HANDLE)
			{
				vkDestroySwapchainKHR(m_logicalDevice, m_swapChain, nullptr);
				vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
			}
			m_surface = VK_NULL_HANDLE;
			m_swapChain = VK_NULL_HANDLE;
		}


	}



}



#endif // !VULKAN_SWAPCHAIN_H_