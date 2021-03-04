#pragma once
#ifndef CRAFT_ENGINE_VULKAN_HIGH_LEVEL_H_
#define CRAFT_ENGINE_VULKAN_HIGH_LEVEL_H_


#include "./VulkanApi.h"
#include "./VulkanConstructor.h"


#include <string>
#include <iostream>
#include <vector>
#include <mutex>

#include <stdexcept>

#undef NDEBUG
#include <assert.h>

//#include <vulkan/vulkan.hpp>

#define VKAPI_throw_runtime_error(errcode, errarg)		                             \
{																					 \
	std::cout << "A runtime error is occurred! "<< __LINE__ << std::endl;			 \
	std::cout << "Error code("<< errcode << "):" << (const char*)errarg << std::endl;\
	system("pause");																 \
	assert(0);																		 \
};																					 


#undef max
#undef min

#include <math.h>

namespace CraftEngine
{


	namespace vulkan
	{

		enum {
			MAX_PHYSICAL_DEVICE = 2,
			MAX_LOGICAL_DEVICE_QUEUE = 16,
		};

		enum class ImageFormat : int32_t
		{
			format_r8_snorm = VK_FORMAT_R8_SNORM,
			format_r8_unorm = VK_FORMAT_R8_UNORM,
			format_rgba8_unorm = VK_FORMAT_R8G8B8A8_UNORM,
			format_rgba8_snorm = VK_FORMAT_R8G8B8A8_SNORM,
			format_rgba16_unorm = VK_FORMAT_R16G16B16A16_UNORM,
			format_rgba16_snorm = VK_FORMAT_R16G16B16A16_SNORM,
			format_rgba16_sfloat = VK_FORMAT_R16G16B16A16_SFLOAT,
			format_d24_snorm_s8_uint = VK_FORMAT_D24_UNORM_S8_UINT,
			format_d32_sfloat_s8_uint = VK_FORMAT_D32_SFLOAT_S8_UINT,
			format_d32_sfloat = VK_FORMAT_D32_SFLOAT,
		};


		inline void makeCmdBegin(VkCommandBuffer commandBuffer, VkCommandBufferUsageFlagBits flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)
		{
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = flags;
			vkBeginCommandBuffer(commandBuffer, &beginInfo);
		}

		inline void makeCmdEnd(VkCommandBuffer commandBuffer)
		{
			vkEndCommandBuffer(commandBuffer);
		}


		inline void makeCmdTransitionImageLayout(
			VkCommandBuffer commandBuffer,
			VkImage image,
			VkImageLayout oldImageLayout,
			VkImageLayout newImageLayout,
			VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
			VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
		) {
			VkImageMemoryBarrier imageMemoryBarrier{};
			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.oldLayout = oldImageLayout;
			imageMemoryBarrier.newLayout = newImageLayout;
			imageMemoryBarrier.image = image;
			imageMemoryBarrier.subresourceRange = subresourceRange;

			// Source layouts (old)
			// Source access mask controls actions that have to be finished on the old layout
			// before it will be transitioned to the new layout
			switch (oldImageLayout)
			{
			case VK_IMAGE_LAYOUT_UNDEFINED:
				// Image layout is undefined (or does not matter)
				// Only valid as initial layout
				// No flags required, listed only for completeness
				imageMemoryBarrier.srcAccessMask = 0;
				break;

			case VK_IMAGE_LAYOUT_PREINITIALIZED:
				// Image is preinitialized
				// Only valid as initial layout for linear images, preserves memory contents
				// Make sure host writes have been finished
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				// Image is a color attachment
				// Make sure any writes to the color buffer have been finished
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				// Image is a depth/stencil attachment
				// Make sure any writes to the depth/stencil buffer have been finished
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				// Image is a transfer source 
				// Make sure any reads from the image have been finished
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				// Image is a transfer destination
				// Make sure any writes to the image have been finished
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				// Image is read by a shader
				// Make sure any shader reads from the image have been finished
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
				break;
			default:
				// Other source layouts aren't handled (yet)
				break;
			}

			// Target layouts (new)
			// Destination access mask controls the dependency for the new image layout
			switch (newImageLayout)
			{
			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				// Image will be used as a transfer destination
				// Make sure any writes to the image have been finished
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				// Image will be used as a transfer source
				// Make sure any reads from the image have been finished
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				// Image will be used as a color attachment
				// Make sure any writes to the color buffer have been finished
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				// Image layout will be used as a depth/stencil attachment
				// Make sure any writes to depth/stencil buffer have been finished
				imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				// Image will be read in a shader (sampler, input attachment)
				// Make sure any writes to the image have been finished
				if (imageMemoryBarrier.srcAccessMask == 0)
				{
					imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
				}
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				break;
			default:
				// Other source layouts aren't handled (yet)
				break;
			}

			// Put barrier inside setup command buffer
			vkCmdPipelineBarrier(
				commandBuffer,
				srcStageMask,
				dstStageMask,
				0,
				0, nullptr,
				0, nullptr,
				1, &imageMemoryBarrier);
		}


		inline void makeCmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkImage dstImage, VkBuffer srcBuffer, uint32_t imageWidth, uint32_t imageHeight = 1U, uint32_t imageDepth = 1U)
		{
			VkBufferImageCopy region = {};
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;
			region.imageOffset = { 0, 0, 0 };
			region.imageExtent = { imageWidth,imageHeight, imageDepth };
			vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
		}

		inline void makeCmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkImage srcImage, uint32_t imageWidth, uint32_t imageHeight = 1U, uint32_t imageDepth = 1U)
		{
			VkBufferImageCopy region = {};
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;
			region.imageOffset = { 0, 0, 0 };
			region.imageExtent = { imageWidth,imageHeight, imageDepth };
			vkCmdCopyImageToBuffer(commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, dstBuffer, 1, &region);
		}


		inline void makeCmdCopyBufferToBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkBuffer srcBuffer, VkDeviceSize size, uint32_t dstOffset = 0U, uint32_t srcOffset = 0U)
		{
			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = srcOffset;
			copyRegion.dstOffset = dstOffset;
			copyRegion.size = size;
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
		}


		struct BufferReference
		{
			VkBuffer buffer;
			uint32_t size;
		};

		struct ImageReference
		{
			VkImage image;
			VkImageView view;
			VkFormat format;
		};



		bool isDepthFormat(ImageFormat format_enum)
		{
			return
				format_enum == ImageFormat::format_d32_sfloat ||
				format_enum == ImageFormat::format_d24_snorm_s8_uint ||
				format_enum == ImageFormat::format_d32_sfloat_s8_uint;
		}
		bool haveStencilComponont(ImageFormat format_enum)
		{
			if (format_enum == ImageFormat::format_d24_snorm_s8_uint || format_enum == ImageFormat::format_d32_sfloat_s8_uint)
				return true;
			else
				return false;
		}
		bool haveStencilComponont(VkFormat format_enum)
		{
			return haveStencilComponont((ImageFormat)format_enum);
		}
		bool isDepthFormat(VkFormat format_enum)
		{
			return isDepthFormat((ImageFormat)format_enum);
		}


		struct ImageTexture
		{
			VkImage mImage;
			VkDeviceMemory mMemory;
			VkImageView mView;
			VkImageView mDepthView;
			VkImageView mStencilView;
			VkFormat mFormat;
			uint32_t mWidth;
			uint32_t mHeight;
			uint32_t mDepth;
			uint32_t mMipLevel;
			VkSampleCountFlagBits mSampleCount;

			ImageReference reference() { return { mImage, mView, mFormat }; }
			ImageReference depthReference() { return { mImage, mDepthView, mFormat }; }
			ImageReference stencilReference() { return { mImage, mStencilView, mFormat }; }
		};



		struct BufferMemory
		{
			VkBuffer       mBuffer;
			VkDeviceMemory mMemory;
			void*          mAddress;
			uint32_t       mSize;
			void* data()const
			{
				return mAddress;
			}
			BufferReference reference()const
			{
				return { mBuffer, mSize };
			}
			bool write(const void* data, uint32_t size)
			{
				memcpy(mAddress, data, size);
			}
			template<typename Type>
			void operator=(const Type& data)
			{
				write(&data, sizeof(Type));
			}
		};




		//template<typename type, uint32_t Length>
		//class UniformBufferObjectPool
		//{
		//private:
		//	friend class LogicalDevice;
		//	VkBuffer* m_pBuffers;
		//	void* m_pMemory;
		//	uint32_t m_offset;
		//	uint32_t m_count;
		//	VkDeviceMemory m_memory;
		//	core::Pool<BufferReference, Length>* m_bufferRefs;

		//	inline uint32_t count()const
		//	{
		//		return m_count;
		//	}
		//	inline type* pointer(const uint32_t index)
		//	{
		//		return (type*)((char*)m_pMemory + (m_offset * index));
		//	}
		//	inline VkBuffer buffer(uint32_t index)const
		//	{
		//		return m_pBuffers[index];
		//	}
		//public:

		//	BufferReference* alloc()
		//	{
		//		auto bfref = m_bufferRefs->alloc();
		//		auto index = m_bufferRefs->indexOf(bfref);
		//		bfref->buffer = buffer(index);
		//		bfref->pointer = pointer(index);
		//		bfref->size = sizeof(type);
		//		return bfref;
		//	}

		//	void free(BufferReference* buf)
		//	{
		//		m_bufferRefs->free(buf);
		//	}

		//};



		struct DescriptorPool
		{
		private:
			friend class LogicalDevice;
			VkDevice         mVulkanLogicalDeviceObject;
			VkDescriptorPool mVulkanDescriptorPoolObject;
		public:
			// 分配描述符集合
			inline void create(VkDescriptorSetLayout* pDescriptorSetLayouts, VkDescriptorSet* pDescriptorSets, uint32_t count = 1)
			{
				VkDescriptorSetAllocateInfo allocInfo = {};
				allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				allocInfo.descriptorPool = mVulkanDescriptorPoolObject;
				allocInfo.descriptorSetCount = count;
				allocInfo.pSetLayouts = pDescriptorSetLayouts;
				if (vkAllocateDescriptorSets(mVulkanLogicalDeviceObject, &allocInfo, pDescriptorSets) != VK_SUCCESS)
					VKAPI_throw_runtime_error(0, "failed to allocate descriptor set!");
			}

			// 释放描述符集合
			inline void destroy(VkDescriptorSet* pDescriptorSets, uint32_t count = 1)
			{
				vkFreeDescriptorSets(mVulkanLogicalDeviceObject, mVulkanDescriptorPoolObject, count, pDescriptorSets);
			}
		};



		struct CommandPool
		{
		private:
			friend class LogicalDevice;
			VkCommandPool mVulkanCommandPoolObject;
			VkDevice      mVulkanLogicalDeviceObject;
		public:


			// 分配命令缓冲区
			inline void create(uint32_t count, VkCommandBuffer* pCommandBuffers, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY)const
			{
				VkCommandBufferAllocateInfo allocInfo = {};
				allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocInfo.commandPool = mVulkanCommandPoolObject;
				allocInfo.level = level;
				allocInfo.commandBufferCount = count;
				if (vkAllocateCommandBuffers(mVulkanLogicalDeviceObject, &allocInfo, pCommandBuffers) != VK_SUCCESS)
					VKAPI_throw_runtime_error(0, "failed to allocate command buffers!");
			}
			inline void destroy(const VkCommandBuffer* pCommandBuffers, uint32_t count = 1)const
			{
				vkFreeCommandBuffers(mVulkanLogicalDeviceObject, mVulkanCommandPoolObject, count, pCommandBuffers);
			}
		};


		struct QueryPool
		{
		private:
			friend class LogicalDevice;
			VkDevice      mVulkanLogicalDeviceObject;
			VkQueryPool   mVulkanQueryPoolObject;
		public:


		};






		class Instance
		{
		private:
			// Vulkan 实例
			VkInstance m_instance;

			VkPhysicalDevice           m_allPhysicalDevices[MAX_PHYSICAL_DEVICE];
			VkPhysicalDeviceProperties m_allPhysicalDeviceProperties[MAX_PHYSICAL_DEVICE];
			VkPhysicalDeviceFeatures   m_allPhysicalDeviceFeatures[MAX_PHYSICAL_DEVICE];
			uint32_t                   m_allPhysicalDeviceCount;

			bool                     m_enableDebugReport = false;
			VkDebugReportCallbackEXT m_debugCallback = VK_NULL_HANDLE;

			static VKAPI_ATTR VkBool32 VKAPI_CALL vulkanDebugReportCallback(
				VkDebugReportFlagsEXT flags,
				VkDebugReportObjectTypeEXT objType,
				uint64_t obj,
				size_t location,
				int32_t code,
				const char* layerPrefix,
				const char* msg,
				void* userData
			)
			{
				std::cerr << "validation layer: " << msg << std::endl << std::endl;
				//std::cerr << "continue?[yes(y)/no(n)]: " << msg << std::endl << std::endl;
				//std::string cmd;
				//for (auto& it : cmd)
				//	it = tolower(it);
				//std::cin >> cmd;
				//if (cmd == "1" || cmd == "y" || cmd == "yes")
				//{

				//}
				//else
				//{
				//	VKAPI_throw_runtime_error(code, "validation");
				//}	
				return VK_FALSE;
			}
		public:

			// Instance
			Instance(
				const char* appName,
				const char* engineName,
				std::vector<const char*> enabledLayerNames,
				std::vector<const char*> enabledExtensionNames
			) {
				VkApplicationInfo appInfo = {};
				appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
				appInfo.pApplicationName = appName;
				appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
				appInfo.pEngineName = engineName;
				appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
				appInfo.apiVersion = VK_API_VERSION_1_0;

				VkInstanceCreateInfo vulkanInstanceCreateInfo = {};
				vulkanInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
				vulkanInstanceCreateInfo.pApplicationInfo = &appInfo;

				// 启用拓展
				vulkanInstanceCreateInfo.enabledExtensionCount = enabledExtensionNames.size();
				vulkanInstanceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames.data();

				// 启用层
				vulkanInstanceCreateInfo.enabledLayerCount = enabledLayerNames.size();
				vulkanInstanceCreateInfo.ppEnabledLayerNames = enabledLayerNames.data();

				// 使用25MB内存
				if (vkCreateInstance(&vulkanInstanceCreateInfo, nullptr, &m_instance) != VK_SUCCESS) {
					throw std::runtime_error("failed to create instance!");
				}

				uint32_t deviceCount = 0;
				vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

				if (deviceCount == 0) {
					throw std::runtime_error("failed to find GPUs with Vulkan support!");
				}

				m_allPhysicalDeviceCount = deviceCount < MAX_PHYSICAL_DEVICE ? deviceCount : MAX_PHYSICAL_DEVICE;

				std::cout << "Vulkan Message:" << "[" << m_allPhysicalDeviceCount << "] Physical Devices(Device) were(was) found." << std::endl;

				vkEnumeratePhysicalDevices(m_instance, &m_allPhysicalDeviceCount, m_allPhysicalDevices);
				for (int i = 0; i < deviceCount; i++)
				{
					vkGetPhysicalDeviceProperties(m_allPhysicalDevices[i], &m_allPhysicalDeviceProperties[i]);
					vkGetPhysicalDeviceFeatures(m_allPhysicalDevices[i], &m_allPhysicalDeviceFeatures[i]);
				}
			}

			~Instance()
			{
				if (m_debugCallback != VK_NULL_HANDLE)
				{
					auto func = (PFN_vkDestroyDebugReportCallbackEXT)getInstanceProcAddr("vkDestroyDebugReportCallbackEXT");
					func(m_instance, m_debugCallback, nullptr);
				}
				vkDestroyInstance(m_instance, nullptr);
			}


			inline const VkInstance& getInstance()const { return m_instance; }
			inline const VkPhysicalDevice& getPhysicalDevice(int index) { return m_allPhysicalDevices[index]; }
			inline int   getPhysicalDeviceCount()const { return m_allPhysicalDeviceCount; }
			inline const VkPhysicalDeviceProperties& getPhysicalDeviceProperties(int index)const { return m_allPhysicalDeviceProperties[index]; }
			inline const VkPhysicalDeviceFeatures& getPhysicaleviceFeatures(int index)const { return m_allPhysicalDeviceFeatures[index]; }

			// 获取实例函数地址
			inline void* getInstanceProcAddr(const char* procName) const
			{
				void* proc = vkGetInstanceProcAddr(m_instance, procName);
				if (proc == NULL)
				{
					throw std::runtime_error("不能获取函数" + std::string(procName) + "的地址（请检查是否开启对应拓展）");
				}
				return proc;
			}

			inline int enableDebugReport()
			{
				if (m_enableDebugReport == true)
					return VK_SUCCESS;

				VkDebugReportCallbackCreateInfoEXT vulkanDebugReportCallbackCreateInfo = {};
				vulkanDebugReportCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
				vulkanDebugReportCallbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
				vulkanDebugReportCallbackCreateInfo.pfnCallback = vulkanDebugReportCallback;

				auto func = (PFN_vkCreateDebugReportCallbackEXT)getInstanceProcAddr("vkCreateDebugReportCallbackEXT");
				if (func != nullptr) {
					m_enableDebugReport = true;
					return func(m_instance, &vulkanDebugReportCallbackCreateInfo, nullptr, &m_debugCallback);
				}
				else {
					return VK_ERROR_EXTENSION_NOT_PRESENT;
				}
			}


			int getBestPhysicalDeviceIndex()
			{
				int selectedPhysicalDeviceIndex = 0;
				std::vector<int> scores;
				for (int i = 0; i < getPhysicalDeviceCount(); i++)
				{
					int score = 0;
					switch (getPhysicalDeviceProperties(i).deviceType)
					{
					case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:score += 10000; break;
					default:break;
					}
					scores.push_back(score);
				}
				int scoreMax = scores[0];
				for (int i = 1; i < getPhysicalDeviceCount(); i++)
				{
					if (scoreMax < scores[i])
					{
						scoreMax = scores[i];
						selectedPhysicalDeviceIndex = i;
					}
				}
				return selectedPhysicalDeviceIndex;
			}

		};



		class PhysicalDevice
		{
		private:
			friend class Instance;
			Instance* m_instance;

			VkPhysicalDevice                     m_physicalDevice;
			VkPhysicalDeviceProperties           m_properties;
			VkPhysicalDeviceFeatures             m_features;
			VkPhysicalDeviceMemoryProperties     m_memoryProperties;
			uint32_t                             m_queueFamilyIndex;
			VkQueueFamilyProperties              m_queueFamilyProperties;

			VkSampleCountFlags                   m_maxMsaaSampleCount;

			VkSampleCountFlags findMaxMSAASampleCount()
			{
				VkSampleCountFlags counts = m_properties.limits.framebufferColorSampleCounts;
				if (m_properties.limits.framebufferDepthSampleCounts < counts)
					counts = m_properties.limits.framebufferDepthSampleCounts;
				if (counts & VK_SAMPLE_COUNT_64_BIT)
					return VK_SAMPLE_COUNT_64_BIT;
				if (counts & VK_SAMPLE_COUNT_32_BIT)
					return VK_SAMPLE_COUNT_32_BIT;
				if (counts & VK_SAMPLE_COUNT_16_BIT)
					return VK_SAMPLE_COUNT_16_BIT;
				if (counts & VK_SAMPLE_COUNT_8_BIT)
					return VK_SAMPLE_COUNT_8_BIT;
				if (counts & VK_SAMPLE_COUNT_4_BIT)
					return VK_SAMPLE_COUNT_4_BIT;
				if (counts & VK_SAMPLE_COUNT_2_BIT)
					return VK_SAMPLE_COUNT_2_BIT;
				return VK_SAMPLE_COUNT_1_BIT;
			}

		public:

			// PhysicalDevice
			PhysicalDevice(int index, Instance* instance)
			{
				m_instance = instance;
				if (index < 0 || !(index < m_instance->getPhysicalDeviceCount()))
				{
					VKAPI_throw_runtime_error(0, "选择的设备不在可选范围内！");
				}
				{
					std::cout << "Vulkan Message:" << "Enable GPU[" << std::to_string(index) << "][" << getParent()->getPhysicalDeviceProperties(index).deviceName << "]." << std::endl;
					std::cout << "Vulkan Message:" << "Api Version[" << getParent()->getPhysicalDeviceProperties(index).apiVersion << "]." << std::endl;
				}
				VkPhysicalDeviceMemoryProperties memoryProperties = {};
				vkGetPhysicalDeviceMemoryProperties(getParent()->getPhysicalDevice(index), &memoryProperties);

				uint32_t queueFamilyCount = 0;
				int32_t  queueFamilyIndex = -1;

				vkGetPhysicalDeviceQueueFamilyProperties(getParent()->getPhysicalDevice(index), &queueFamilyCount, nullptr);
				VkQueueFamilyProperties* queueFamilyProperties = new VkQueueFamilyProperties[queueFamilyCount];
				vkGetPhysicalDeviceQueueFamilyProperties(getParent()->getPhysicalDevice(index), &queueFamilyCount, queueFamilyProperties);
				for (int i = 0; i < queueFamilyCount; i++)
				{
					if (
						(queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
						(queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) &&
						(queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
						)
					{
						queueFamilyIndex = i;
						break;
					}
				}
				if (queueFamilyIndex == -1)
				{
					VKAPI_throw_runtime_error(0, "不能在当前设备找到通用的队列族！");
				}
				std::cout << "Vulkan Message:" << "QueueFamileIndex[" << queueFamilyIndex << "]." << std::endl;

				m_physicalDevice = getParent()->getPhysicalDevice(index);
				m_properties = getParent()->getPhysicalDeviceProperties(index);
				m_features = getParent()->getPhysicaleviceFeatures(index);
				m_memoryProperties = memoryProperties;
				m_queueFamilyIndex = queueFamilyIndex;
				m_queueFamilyProperties = queueFamilyProperties[queueFamilyIndex];

				m_maxMsaaSampleCount = findMaxMSAASampleCount();

				delete[] queueFamilyProperties;
			}


			PhysicalDevice(Instance* instance) :PhysicalDevice(instance->getBestPhysicalDeviceIndex(), instance)
			{

			}

			~PhysicalDevice()
			{

			}

			inline Instance* getParent()const { return m_instance; }
			inline const VkPhysicalDevice& getPhysicalDevice()const { return m_physicalDevice; }
			inline const VkPhysicalDeviceProperties& getProperties()const { return m_properties; }
			inline const VkPhysicalDeviceFeatures& getFeatures()const { return m_features; }
			inline const VkPhysicalDeviceMemoryProperties& getMemoryProperties()const { return m_memoryProperties; }
			inline uint32_t getQueueFamilyIndex()const { return m_queueFamilyIndex; }
			inline const VkQueueFamilyProperties& getQueueFamilyProperties()const { return m_queueFamilyProperties; }

			inline VkFormatProperties getFormatProperties(VkFormat format)const
			{
				VkFormatProperties props;
				vkGetPhysicalDeviceFormatProperties(getPhysicalDevice(), format, &props);
				return props;
			}

			VkSampleCountFlags getMaxMSAASampleCount()
			{
				return m_maxMsaaSampleCount;
			}
		};




		class LogicalDevice
		{
		private:
			friend class PhysicalDevice;
			PhysicalDevice* m_physicalDevice;

			// 逻辑设备
			VkDevice      m_logicalDevice;
			std::mutex    m_logicalDeviceMutex;

			// 队列
			VkQueue       m_queues[MAX_LOGICAL_DEVICE_QUEUE];
			std::mutex    m_queueMutexs[MAX_LOGICAL_DEVICE_QUEUE];
			float         m_queuePriorities[MAX_LOGICAL_DEVICE_QUEUE];
			uint32_t      m_queueCount;

			struct QueueInfo
			{
				int m_graphicsQueueIndex;
				int m_transferQueueIndex;
				int m_presentQueueIndex;
				int m_computeQueueIndex;
			}m_queueInfo;

			VkPhysicalDeviceProperties           m_physicalDeviceProperties;
			VkPhysicalDeviceFeatures             m_physicalDeviceFeatures;
			VkPhysicalDeviceMemoryProperties     m_physicalDeviceMemoryProperties;
			uint32_t                             m_physicalDeviceQueueFamilyIndex;
			VkQueueFamilyProperties              m_physicalDeviceQueueFamilyProperties;

		public:

			LogicalDevice(
				int queue_count,
				std::vector<const char*> enabledLayerNames,
				std::vector<const char*> enabledExtensionNames,
				VkPhysicalDeviceFeatures enableDeviceFeatures,
				PhysicalDevice* physical_device
			)
			{
				m_physicalDevice = physical_device;


				int reqQueueCount = queue_count > getParent()->getQueueFamilyProperties().queueCount ? getParent()->getQueueFamilyProperties().queueCount : queue_count;

				if (reqQueueCount <= 0)
					VKAPI_throw_runtime_error(0, "不能少于1个队列！");
				if (reqQueueCount > getParent()->getQueueFamilyProperties().queueCount)
					throw std::runtime_error("申请的队列数量超过上限！（最大为：" + std::to_string(getParent()->getQueueFamilyProperties().queueCount) + "）");
				if (reqQueueCount > MAX_LOGICAL_DEVICE_QUEUE)
					throw std::runtime_error("申请的队列数量超过上限！（最大为：" + std::to_string(getParent()->getQueueFamilyProperties().queueCount) + "）");


				m_queueCount = reqQueueCount;
				for (int i = 0; i < reqQueueCount; i++)
					m_queuePriorities[i] = 1.0f;


				VkDeviceQueueCreateInfo queueCreateInfo = {};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = m_physicalDevice->getQueueFamilyIndex();
				queueCreateInfo.queueCount = reqQueueCount;
				queueCreateInfo.pQueuePriorities = m_queuePriorities;

				m_queueInfo.m_graphicsQueueIndex = 0;
				if (m_queueCount > 1)
					m_queueInfo.m_transferQueueIndex = 1;
				else
					m_queueInfo.m_transferQueueIndex = 0;

				if (m_queueCount > 2)
					m_queueInfo.m_presentQueueIndex = 2;
				else if (m_queueCount == 2)
					m_queueInfo.m_presentQueueIndex = 1;
				else
					m_queueInfo.m_presentQueueIndex = 0;


				if (m_physicalDevice->getFeatures().tessellationShader == VK_FALSE)
					std::cout << "不支持曲面细分着色器" << std::endl;

				// 开启特性
				{
					enableDeviceFeatures.sampleRateShading = VK_TRUE;
					enableDeviceFeatures.samplerAnisotropy = VK_TRUE;
					enableDeviceFeatures.fillModeNonSolid = VK_TRUE;
					enableDeviceFeatures.alphaToOne = VK_TRUE;
					enableDeviceFeatures.wideLines = VK_TRUE;
				}


				VkDeviceCreateInfo logicDeviceCreateInfo = {};
				logicDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
				logicDeviceCreateInfo.queueCreateInfoCount = 1;
				logicDeviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
				logicDeviceCreateInfo.pEnabledFeatures = &enableDeviceFeatures;
				logicDeviceCreateInfo.enabledExtensionCount = enabledExtensionNames.size();
				logicDeviceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames.data();
				logicDeviceCreateInfo.enabledLayerCount = enabledLayerNames.size();
				logicDeviceCreateInfo.ppEnabledLayerNames = enabledLayerNames.data();

				// 18MB 内存
				VkResult errorCode = vkCreateDevice(m_physicalDevice->getPhysicalDevice(), &logicDeviceCreateInfo, nullptr, &m_logicalDevice);
				
				if (errorCode != VK_SUCCESS)
					throw std::runtime_error("创建逻辑设备失败！错误代码（" + std::to_string(errorCode) + "）。");

				for (int i = 0; i < reqQueueCount; i++)
					vkGetDeviceQueue(m_logicalDevice, m_physicalDevice->getQueueFamilyIndex(), i, &m_queues[i]);

				m_physicalDeviceFeatures = m_physicalDevice->getFeatures();
				m_physicalDeviceMemoryProperties = m_physicalDevice->getMemoryProperties();
				m_physicalDeviceQueueFamilyIndex = m_physicalDevice->getQueueFamilyIndex();
				m_physicalDeviceQueueFamilyProperties = m_physicalDevice->getQueueFamilyProperties();
				m_physicalDeviceProperties = m_physicalDevice->getProperties();


			}

			~LogicalDevice()
			{
				vkDestroyDevice(getLogicalDevice(), nullptr);
			}


			inline PhysicalDevice* getParent()const { return m_physicalDevice; }
			inline const VkDevice getLogicalDevice()const { return m_logicalDevice; }
			inline const void lockQueue(uint32_t index) { m_queueMutexs[index].lock(); }
			inline const void unlockQueue(uint32_t index) { m_queueMutexs[index].unlock(); }
			inline const VkQueue& getQueue(uint32_t index)const { return m_queues[index]; }

			int getGraphicsQueueIndex() { return m_queueInfo.m_graphicsQueueIndex; }
			int getTransferQueueIndex() { return m_queueInfo.m_transferQueueIndex; }
			int getPresentQueueIndex() { return m_queueInfo.m_presentQueueIndex; }

			inline void waitQueue(uint32_t index) { std::lock_guard<std::mutex> lock(m_queueMutexs[index]);  vkQueueWaitIdle(m_queues[index]); }
			inline void waitDevice() { std::lock_guard<std::mutex> lock(m_logicalDeviceMutex); vkDeviceWaitIdle(m_logicalDevice); }

			// PhysicalDevice
			inline const VkPhysicalDeviceProperties& getPhysicalDeviceProperties()const { return m_physicalDeviceProperties; }
			inline const VkPhysicalDeviceFeatures& getPhysicalDeviceFeatures()const { return m_physicalDeviceFeatures; }
			inline const VkPhysicalDeviceMemoryProperties& getPhysicalDeviceMemoryProperties()const { return m_physicalDeviceMemoryProperties; }
			inline uint32_t                                getPhysicalDeviceQueueFamilyIndex()const { return m_physicalDeviceQueueFamilyIndex; }
			inline const VkQueueFamilyProperties& getPhysicalDeviceQueueFamilyProperties()const { return m_physicalDeviceQueueFamilyProperties; }


			// 查找内存类型索
			inline uint32_t getMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties)const {
				for (uint32_t index = 0; index < getPhysicalDeviceMemoryProperties().memoryTypeCount; index++)
					if ((typeFilter & (1 << index)) && (getPhysicalDeviceMemoryProperties().memoryTypes[index].propertyFlags & properties) == properties)
						return index;
				VKAPI_throw_runtime_error(0, "不能找到合适的内存类型!");
			}
		private:
			inline VkMemoryRequirements GetBufferMemoryRequirements(VkBuffer buffer)const
			{
				VkMemoryRequirements memRequirements = {};
				vkGetBufferMemoryRequirements(m_logicalDevice, buffer, &memRequirements);
				return memRequirements;
			}

			inline VkMemoryRequirements GetImageMemoryRequirements(VkImage image)const
			{
				VkMemoryRequirements memRequirements;
				vkGetImageMemoryRequirements(m_logicalDevice, image, &memRequirements);
				return memRequirements;
			}
		private:

			// 创建缓存
			inline VkBuffer CreateVulkanBuffer(const VkBufferCreateInfo& info)const
			{
				VkBuffer buffer;
				if (vkCreateBuffer(m_logicalDevice, &info, nullptr, &buffer) != VK_SUCCESS)
					VKAPI_throw_runtime_error(0, "failed to create buffer!");
				return buffer;
			}

			// 创建缓存
			inline VkImage CreateVulkanImage(const VkImageCreateInfo& info)const
			{
				VkImage image;
				if (vkCreateImage(m_logicalDevice, &info, nullptr, &image) != VK_SUCCESS)
					VKAPI_throw_runtime_error(0, "failed to create image!");
				return image;
			}

			// 分配图像
			inline VkDeviceMemory CreateVulkanDeviceMemory(const VkMemoryAllocateInfo& info)const
			{
				VkDeviceMemory memory;
				if (vkAllocateMemory(m_logicalDevice, &info, nullptr, &memory) != VK_SUCCESS)
					VKAPI_throw_runtime_error(0, "failed to allocate device memory!");
				return memory;
			}

			inline VkImageView CreateVulkanImageView(const VkImageViewCreateInfo& info)const
			{
				VkImageView imageView;
				if (vkCreateImageView(m_logicalDevice, &info, nullptr, &imageView) != VK_SUCCESS)
					VKAPI_throw_runtime_error(0, "failed to create texture image view!");
				return imageView;
			}

			inline void DestroyBuffer(VkBuffer buffer)const;
			inline void DestroyImage(VkImage image)const;
			inline void DestroyImageView(VkImageView imageView)const;
			inline void DestroyMemory(VkDeviceMemory memory)const;


		public:

			inline void* getDeviceProcAddr(const char* procName)const
			{
				void* proc = vkGetDeviceProcAddr(m_logicalDevice, procName);
				if (proc == NULL)
					VKAPI_throw_runtime_error(0, "不能获取函数" + std::string(procName) + "的地址（请检查是否开启对应拓展）");
				return proc;
			}

			// 创建渲染通道
			inline VkRenderPass createRenderPass(
				VkRenderPassCreateFlags flags, 
				uint32_t attachmentCount, 
				const VkAttachmentDescription* pAttachments, 
				uint32_t subpassCount,
				const VkSubpassDescription* pSubpasses,
				uint32_t dependencyCount, 
				const VkSubpassDependency* pDependencies) const
			{
				VkRenderPassCreateInfo info = {};
				info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				info.pNext = nullptr;
				info.flags = flags;
				info.attachmentCount = attachmentCount;
				info.pAttachments = pAttachments;
				info.subpassCount = subpassCount;
				info.pSubpasses = pSubpasses;
				info.dependencyCount = dependencyCount;
				info.pDependencies = pDependencies;
				VkRenderPass renderPass;
				if (vkCreateRenderPass(m_logicalDevice, &info, nullptr, &renderPass) != VK_SUCCESS)
					VKAPI_throw_runtime_error(0, "failed to create render pass!");
				return renderPass;
			}


			// 提交命令
			inline void submitCommands(
				uint32_t queueIndex,
				uint32_t commandBufferCount,
				const VkCommandBuffer* pCmdBufs,
				uint32_t waitSemaphoreCount,
				const VkSemaphore* pWaitSemaphore,
				const VkPipelineStageFlags* pWaitDstStageMask,
				uint32_t signalSemaphoreCount,
				const VkSemaphore* pSignalSemaphores,
				VkFence fence = VK_NULL_HANDLE
			) {
				VkSubmitInfo submitInfo = {};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.waitSemaphoreCount = waitSemaphoreCount;
				submitInfo.pWaitSemaphores = pWaitSemaphore;
				submitInfo.pWaitDstStageMask = pWaitDstStageMask;
				submitInfo.commandBufferCount = commandBufferCount;
				submitInfo.pCommandBuffers = pCmdBufs;
				submitInfo.signalSemaphoreCount = signalSemaphoreCount;
				submitInfo.pSignalSemaphores = pSignalSemaphores;
				VkResult result = VK_SUCCESS;
				{
					std::lock_guard<std::mutex> lock(m_queueMutexs[queueIndex]);
					VkResult result = vkQueueSubmit(getQueue(queueIndex), 1, &submitInfo, fence);
				}
				if (result != VK_SUCCESS)
					VKAPI_throw_runtime_error(result, "提交命令缓冲失败！");
			}

			void submitCommands(
				uint32_t queueIndex,
				const std::vector<VkCommandBuffer>& cmdBufs,
				const std::vector<VkSemaphore>& waitSemaphore,
				const std::vector<VkPipelineStageFlags>& waitDstStageMask,
				const std::vector<VkSemaphore>& signalSemaphores,
				VkFence fence = VK_NULL_HANDLE
			) {
				submitCommands(
					queueIndex,
					(uint32_t)cmdBufs.size(), cmdBufs.data(),
					(uint32_t)waitSemaphore.size(), waitSemaphore.data(), waitDstStageMask.data(),
					(uint32_t)signalSemaphores.size(), signalSemaphores.data(),
					fence
				);
			}



			// 立即执行命令
			inline void executeCommands(uint32_t queueIndex, const VkCommandBuffer* pCommandBuffers, uint32_t count = 1)
			{
				VkResult result;
				VkFence fence = createFence();
				VkSubmitInfo submitInfo = {};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.commandBufferCount = count;
				submitInfo.pCommandBuffers = pCommandBuffers;
				{
					std::lock_guard<std::mutex> lock(m_queueMutexs[queueIndex]);
					vkQueueSubmit(m_queues[queueIndex], 1, &submitInfo, fence);
				}
				waitFence(fence);
				destroyFence(fence);
			}
			inline void executeCommands(uint32_t queueIndex, const std::vector<VkCommandBuffer>& cmdBufs)
			{
				executeCommands(queueIndex, cmdBufs.data(), cmdBufs.size());
			}


			VkFence createFence()
			{
				VkFenceCreateInfo fenceInfo{};
				fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fenceInfo.flags = 0;
				VkResult result;
				VkFence fence;
				if ((result = vkCreateFence(getLogicalDevice(), &fenceInfo, nullptr, &fence)) != VK_SUCCESS)
					VKAPI_throw_runtime_error(result, "Failed to create a Fence!");
				return fence;
			}
			void resetFence(VkFence fence)
			{
				VkResult result;
				if ((result = vkResetFences(getLogicalDevice(), 1, &fence)) != VK_SUCCESS)
					VKAPI_throw_runtime_error(result, "Fence reset error!");
			}
			void waitFence(VkFence fence)
			{
				VkResult result;
				if ((result = vkWaitForFences(getLogicalDevice(), 1, &fence, VK_FALSE, 0XFFFFFFFF)) != VK_SUCCESS)
					VKAPI_throw_runtime_error(result, "Fence Timeout!");
			}
			void waitFences(uint32_t count, VkFence* pFence, bool wait_all = true)
			{
				VkResult result;
				if ((result = vkWaitForFences(getLogicalDevice(), count, pFence, wait_all, 0XFFFFFFFF)) != VK_SUCCESS)
					VKAPI_throw_runtime_error(result, "Fence Timeout!");
			}
			void destroyFence(VkFence fence)
			{
				vkDestroyFence(getLogicalDevice(), fence, nullptr);
			}


		public:


			inline VkSampler createSampler(uint32_t uMaxLod = 1, uint32_t maxAnisotropy = 1, VkSamplerAddressMode samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT)const
			{
				VkSampler sampler;
				VkSamplerCreateInfo samplerInfo = {};
				samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
				samplerInfo.magFilter = VK_FILTER_LINEAR;//VK_FILTER_LINEAR
				samplerInfo.minFilter = VK_FILTER_LINEAR;
				samplerInfo.addressModeU = samplerAddressMode; // VK_SAMPLER_ADDRESS_MODE_REPEAT
				samplerInfo.addressModeV = samplerAddressMode;
				samplerInfo.addressModeW = samplerAddressMode;
				samplerInfo.compareEnable = VK_FALSE;
				samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
				samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
				samplerInfo.unnormalizedCoordinates = VK_FALSE;//

				// 各向异性过滤
				samplerInfo.anisotropyEnable = VK_TRUE;//VK_FALSE VK_TRUE
				samplerInfo.maxAnisotropy = maxAnisotropy < this->getPhysicalDeviceProperties().limits.maxSamplerAnisotropy ? maxAnisotropy : this->getPhysicalDeviceProperties().limits.maxSamplerAnisotropy;//

				// MipMap
				samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				samplerInfo.minLod = 0.0f;
				samplerInfo.maxLod = uMaxLod;
				samplerInfo.mipLodBias = 0.0f;

				if (vkCreateSampler(m_logicalDevice, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
					VKAPI_throw_runtime_error(0, "failed to create texture sampler!");
				return sampler;
			}



			inline VkSemaphore createSemaphore()const
			{
				VkSemaphore semaphore;
				VkSemaphoreCreateInfo semaphoreInfo = {};
				semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				if (vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS)
					VKAPI_throw_runtime_error(0, "failed to create semaphores!");
				return semaphore;
			}


			inline VkShaderModule createShaderModule(const void* code, int size)const
			{
				VkShaderModuleCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.codeSize = size;
				createInfo.pCode = reinterpret_cast<const uint32_t*>(code);
				VkShaderModule shaderModule;
				if (vkCreateShaderModule(m_logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
					VKAPI_throw_runtime_error(0, "failed to create shader module!");
				return shaderModule;
			}


			inline VkPipelineLayout createPipelineLayout(const VkDescriptorSetLayout* descriptorSetLayouts, const uint32_t layoutsCount, uint32_t pushConstantRangeCount = 0, const VkPushConstantRange* pPushConstantRanges = nullptr)const
			{
				VkPipelineLayout pipelineLayout;
				VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
				pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
				pipelineLayoutInfo.setLayoutCount = layoutsCount;
				pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts;
				pipelineLayoutInfo.pPushConstantRanges = pPushConstantRanges;
				pipelineLayoutInfo.pushConstantRangeCount = pushConstantRangeCount;
				if (vkCreatePipelineLayout(m_logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
					VKAPI_throw_runtime_error(0, "failed to create pipeline layout!");
				return pipelineLayout;
			}


			// 为某一个描述符集合创建布局
			inline VkDescriptorSetLayout createDescriptorSetLayout(const VkDescriptorSetLayoutBinding* bindings, uint32_t count)const
			{
				VkDescriptorSetLayout descriptorSetLayout;
				VkDescriptorSetLayoutCreateInfo layoutInfo = {};
				layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layoutInfo.bindingCount = count;
				layoutInfo.pBindings = bindings;
				if (vkCreateDescriptorSetLayout(m_logicalDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
					VKAPI_throw_runtime_error(0, "failed to create descriptor set layout!");
				return descriptorSetLayout;
			}

			// 更新描述符集合
			inline void updateDescriptorSets(uint32_t writesCount, const VkWriteDescriptorSet* descriptorWrites, uint32_t copiesCount, const VkCopyDescriptorSet* descriptorCopies)const {
				vkUpdateDescriptorSets(m_logicalDevice, writesCount, descriptorWrites, copiesCount, descriptorCopies);
			}

			// 帧缓存
			inline VkFramebuffer createFramebuffer(const VkRenderPass& renderPass, uint32_t attachmentCount, const VkImageView* pAttachments, uint32_t width, uint32_t height, uint32_t layers)const
			{
				VkFramebufferCreateInfo framebufferInfo = {};
				framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferInfo.renderPass = renderPass;
				framebufferInfo.attachmentCount = attachmentCount;
				framebufferInfo.pAttachments = pAttachments;
				framebufferInfo.width = width;
				framebufferInfo.height = height;
				framebufferInfo.layers = layers;
				VkFramebuffer framebuffer;
				if (vkCreateFramebuffer(m_logicalDevice, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
					VKAPI_throw_runtime_error(0, "failed to create framebuffer!");
				return framebuffer;
			}


			ImageTexture createColorAttachmentImage(uint32_t width, uint32_t height, ImageFormat colorFormat, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT)const;
			ImageTexture createDepthAttachmentImage(uint32_t width, uint32_t height, ImageFormat depthFormat, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT)const;
			ImageTexture createAttachmentImage(uint32_t width, uint32_t height, ImageFormat format)const
			{
				if (isDepthFormat(format))
					return createDepthAttachmentImage(width, height, format);
				else
					return createColorAttachmentImage(width, height, format);
			}

			inline DescriptorPool createDescriptorPool(const VkDescriptorPoolSize* pPoolSizes, uint32_t poolSizeCount, int maxSets)const
			{
				VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
				VkDescriptorPoolCreateInfo poolInfo = {};
				poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				poolInfo.poolSizeCount = poolSizeCount;
				poolInfo.pPoolSizes = pPoolSizes;
				poolInfo.maxSets = maxSets;
				poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
				if (vkCreateDescriptorPool(m_logicalDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
					VKAPI_throw_runtime_error(0, "failed to create descriptor pool!");
				DescriptorPool object;
				object.mVulkanDescriptorPoolObject = descriptorPool;
				object.mVulkanLogicalDeviceObject = this->getLogicalDevice();
				return object;
			}


			inline CommandPool createCommandPool(VkCommandPoolCreateFlags flags = 0)const
			{
				CommandPool object;
				VkCommandPool commandPool = VK_NULL_HANDLE;
				VkCommandPoolCreateInfo commandPoolCreateInfo = {};
				commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				commandPoolCreateInfo.flags = flags;/////
				commandPoolCreateInfo.queueFamilyIndex = getPhysicalDeviceQueueFamilyIndex();
				if (vkCreateCommandPool(m_logicalDevice, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS)
					VKAPI_throw_runtime_error(0, "无法创建命令池！");

				object.mVulkanLogicalDeviceObject = this->getLogicalDevice();
				object.mVulkanCommandPoolObject = commandPool;

				return object;
			}




			ImageTexture createImage(
				const CommandPool& pool, uint32_t width, uint32_t height, uint32_t depth, ImageFormat format, 
				const void* data = nullptr, uint32_t size = 0, bool mip = false, 
				VkImageUsageFlags usages = (VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT));


			ImageTexture createImage(
				uint32_t width, uint32_t height, uint32_t depth, ImageFormat format,
				VkImageUsageFlags usages = (VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
			);

			ImageTexture createTexture1D(const CommandPool& pool, uint32_t width, ImageFormat format, const void* data = nullptr, uint32_t size = 0, bool mip = false);
			ImageTexture createTexture2D(const CommandPool& pool, uint32_t width, uint32_t height, ImageFormat format, const void* data = nullptr, uint32_t size = 0, bool mip = false);
			ImageTexture createTexture3D(const CommandPool& pool, uint32_t width, uint32_t height, uint32_t depth, ImageFormat format, const void* data = nullptr, uint32_t size = 0, bool mip = false);
			ImageTexture createStorageImage1D(const CommandPool& pool, uint32_t width, ImageFormat format, const void* data, uint32_t size);
			ImageTexture createStorageImage2D(const CommandPool& pool, uint32_t width, uint32_t height, ImageFormat format, const void* data, uint32_t size);
			ImageTexture createStorageImage3D(const CommandPool& pool, uint32_t width, uint32_t height, uint32_t depth, ImageFormat format, const void* data, uint32_t size);

			void updateImage2D(const CommandPool& pool, ImageTexture* pImage, uint32_t width, uint32_t height, const void* data, uint32_t size, ImageFormat format);



			BufferMemory createBuffer(const CommandPool& pool, uint32_t size, VkBufferUsageFlags usages, VkMemoryPropertyFlags mem_property, const void* data = nullptr);


			BufferMemory createHostBuffer(uint32_t size, VkBufferUsageFlags usages, const void* data = nullptr)
			{
				return createBuffer(CommandPool(), size, usages, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, data);
			}
			BufferMemory createDeviceBuffer(uint32_t size, VkBufferUsageFlags usages)
			{
				return createBuffer(CommandPool(), size, usages, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, nullptr);
			}
			BufferMemory createDeviceBuffer(const CommandPool& pool, uint32_t size, VkBufferUsageFlags usages, const void* data = nullptr)
			{
				return createBuffer(pool, size, usages, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, data);
			}


			BufferMemory createUniformBuffer(uint32_t size)
			{
				return createHostBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, nullptr);
			}
			BufferMemory createVertexBuffer(const CommandPool& pool, const void* data, uint32_t size)
			{
				return createBuffer(pool, size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, data);
			}
			BufferMemory createIndexBuffer(const CommandPool& pool, const void* data, uint32_t size)
			{
				return createBuffer(pool, size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, data);
			}

			void copyDataToBuffer(const CommandPool& pool, VkBuffer dst, const void* src, uint32_t size, uint32_t offset = 0);
			void copyBufferToData(const CommandPool& pool, void* dst, VkBuffer src, uint32_t size, uint32_t offset = 0);
			void copyBufferToBuffer(const CommandPool& pool, VkBuffer dst, VkBuffer src, uint32_t size, uint32_t dst_offset = 0U, uint32_t src_offset = 0U);
			void copyImageToBuffer(const CommandPool& pool, VkBuffer dst, VkImage src, uint32_t width, uint32_t height = 1U, uint32_t depth = 1U);
			void destroy(CommandPool commandPool)const;
			void destroy(DescriptorPool descriptorPool)const;
			void destroy(ImageTexture image)const;
			void destroy(BufferMemory buffer)const;





			void destroyFramebuffer(VkFramebuffer framebuffer)const;
			void destroyShaderModule(VkShaderModule shaderModule)const;
			void destroySemaphore(VkSemaphore semaphore)const;
			void destroySampler(VkSampler sampler)const;
			void destroyRenderPass(VkRenderPass renderPass)const;
			void destroyPipeline(VkPipeline pipeline)const;
			void destroyPipelineLayout(VkPipelineLayout pipelineLayout)const;
			void destroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout)const;

		};






		inline void LogicalDevice::DestroyBuffer(VkBuffer buffer)const
		{
			vkDestroyBuffer(m_logicalDevice, buffer, nullptr);
		}

		void CraftEngine::vulkan::LogicalDevice::DestroyImage(VkImage image)const
		{
			vkDestroyImage(m_logicalDevice, image, nullptr);
		}

		inline void LogicalDevice::DestroyImageView(VkImageView imageView) const
		{
			vkDestroyImageView(m_logicalDevice, imageView, nullptr);
		}

		inline void LogicalDevice::DestroyMemory(VkDeviceMemory memory)const
		{
			vkFreeMemory(m_logicalDevice, memory, nullptr);
		}




		// 
		inline ImageTexture LogicalDevice::createColorAttachmentImage(uint32_t width, uint32_t height, ImageFormat colorFormat, VkSampleCountFlagBits sampleCount)const
		{
			//{ VK_FORMAT_R8G8B8A8_UNORM, ... },
			ImageTexture attachmentPackage = {};

			VkImageCreateInfo imageInfo = {};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = width;
			imageInfo.extent.height = height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = (VkFormat)colorFormat;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | ((int(sampleCount) == 1) ? VK_IMAGE_USAGE_STORAGE_BIT : 0);
			imageInfo.samples = sampleCount;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			auto image = CreateVulkanImage(imageInfo);//

			VkMemoryRequirements imageMemRequirements = GetImageMemoryRequirements(image);

			VkMemoryAllocateInfo imageMemoryAllocInfo = {};
			imageMemoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			imageMemoryAllocInfo.allocationSize = imageMemRequirements.size;
			imageMemoryAllocInfo.memoryTypeIndex = getMemoryTypeIndex(imageMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			VkDeviceMemory imageMem = CreateVulkanDeviceMemory(imageMemoryAllocInfo);//
			vkBindImageMemory(getLogicalDevice(), image, imageMem, 0);

			VkImageView imageView;
			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = image;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = (VkFormat)colorFormat;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;
			imageView = CreateVulkanImageView(viewInfo);//

			attachmentPackage.mImage = image;
			attachmentPackage.mMemory = imageMem;
			attachmentPackage.mView = imageView;
			attachmentPackage.mFormat = (VkFormat)colorFormat;
			attachmentPackage.mHeight = height;
			attachmentPackage.mWidth = width;
			attachmentPackage.mDepth = 0;
			attachmentPackage.mDepthView = 0;
			attachmentPackage.mStencilView = 0;
			attachmentPackage.mSampleCount = sampleCount;

			return attachmentPackage;
		}

		inline ImageTexture LogicalDevice::createDepthAttachmentImage(uint32_t width, uint32_t height, ImageFormat depthFormat, VkSampleCountFlagBits sampleCount)const
		{
			ImageTexture attachmentPackage;

			VkImageCreateInfo imageInfo = {};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = width;
			imageInfo.extent.height = height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = (VkFormat)depthFormat;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			imageInfo.samples = sampleCount;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;// 

			auto image = CreateVulkanImage(imageInfo);//

			VkMemoryRequirements imageMemRequirements = GetImageMemoryRequirements(image);

			VkMemoryAllocateInfo imageMemoryAllocInfo = {};
			imageMemoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			imageMemoryAllocInfo.allocationSize = imageMemRequirements.size;
			imageMemoryAllocInfo.memoryTypeIndex = getMemoryTypeIndex(imageMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			VkDeviceMemory imageMem = CreateVulkanDeviceMemory(imageMemoryAllocInfo);//
			vkBindImageMemory(getLogicalDevice(), image, imageMem, 0);

			bool stencil_enable = haveStencilComponont(depthFormat);

			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = image;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = (VkFormat)depthFormat;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | (stencil_enable ? VK_IMAGE_ASPECT_STENCIL_BIT : 0);
			VkImageView image_view = CreateVulkanImageView(viewInfo);//

			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			VkImageView depth_image_view = CreateVulkanImageView(viewInfo);//

			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
			VkImageView stencil_image_view = stencil_enable ? CreateVulkanImageView(viewInfo) : VK_NULL_HANDLE;

			attachmentPackage.mImage = image;
			attachmentPackage.mMemory = imageMem;
			attachmentPackage.mView = image_view;
			attachmentPackage.mDepthView = depth_image_view;
			attachmentPackage.mStencilView = stencil_image_view;
			attachmentPackage.mFormat = (VkFormat)depthFormat;
			attachmentPackage.mHeight = height;
			attachmentPackage.mWidth = width;
			attachmentPackage.mDepth = 0;
			attachmentPackage.mSampleCount = sampleCount;

			return attachmentPackage;
		}


		ImageTexture LogicalDevice::createImage(
			uint32_t width, uint32_t height, uint32_t depth, ImageFormat format,
			VkImageUsageFlags usages
		)
		{		
			VkImageType image_type = depth > 1 ? VK_IMAGE_TYPE_3D : (height > 1 ? VK_IMAGE_TYPE_2D : VK_IMAGE_TYPE_1D);
			VkImageCreateInfo imageInfo = {};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = image_type; // 3D
			imageInfo.extent.width = width;
			imageInfo.extent.height = height;
			imageInfo.extent.depth = depth;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = (VkFormat)format;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = usages;//
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			VkImage image = CreateVulkanImage(imageInfo);//
			VkMemoryRequirements imageMemRequirements = GetImageMemoryRequirements(image);
			VkMemoryAllocateInfo imageMemoryAllocInfo = {};
			imageMemoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			imageMemoryAllocInfo.allocationSize = imageMemRequirements.size;
			imageMemoryAllocInfo.memoryTypeIndex = getMemoryTypeIndex(imageMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			VkDeviceMemory imageMem = CreateVulkanDeviceMemory(imageMemoryAllocInfo);//
			vkBindImageMemory(getLogicalDevice(), image, imageMem, 0);

			VkImageViewType image_view_type = depth > 1 ? VK_IMAGE_VIEW_TYPE_3D : (height > 1 ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_1D);
			VkImageView imageView;
			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = image;
			viewInfo.viewType = image_view_type;
			viewInfo.format = (VkFormat)format;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;
			imageView = CreateVulkanImageView(viewInfo);//

			ImageTexture package = {};
			package.mImage = image;
			package.mMemory = imageMem;
			package.mView = imageView;
			package.mFormat = (VkFormat)format;
			package.mMipLevel = 1;
			package.mHeight = height;
			package.mWidth = width;
			package.mDepth = depth;
			return package;
		}


		inline ImageTexture LogicalDevice::createImage(const CommandPool& pool, uint32_t width, uint32_t height, uint32_t depth, ImageFormat format, const void* data, uint32_t size, bool mip, VkImageUsageFlags usages)
		{
			uint32_t max0 = width > height ? width : height;
			uint32_t max1 = max0 > depth ? max0 : depth;
			uint32_t mipLevels = data ? mip ? (floorf(log2f(max1)) + 1) : 1 : 1;
			VkFormatProperties formatProperties = getParent()->getFormatProperties((VkFormat)format);

			// Mip-chain generation requires support for blit source and destination
			assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT);
			assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT);

			VkImageType image_type = depth > 1 ? VK_IMAGE_TYPE_3D : (height > 1 ? VK_IMAGE_TYPE_2D : VK_IMAGE_TYPE_1D);

			VkImageCreateInfo imageInfo = {};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = image_type; // 3D
			imageInfo.extent.width = width;
			imageInfo.extent.height = height;
			imageInfo.extent.depth = depth;
			imageInfo.mipLevels = mipLevels;
			imageInfo.arrayLayers = 1;
			imageInfo.format = (VkFormat)format;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = usages;//
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			VkImage image = CreateVulkanImage(imageInfo);//
			VkMemoryRequirements imageMemRequirements = GetImageMemoryRequirements(image);
			VkMemoryAllocateInfo imageMemoryAllocInfo = {};
			imageMemoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			imageMemoryAllocInfo.allocationSize = imageMemRequirements.size;
			imageMemoryAllocInfo.memoryTypeIndex = getMemoryTypeIndex(imageMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			VkDeviceMemory imageMem = CreateVulkanDeviceMemory(imageMemoryAllocInfo);//
			vkBindImageMemory(getLogicalDevice(), image, imageMem, 0);

			if (data != nullptr)
			{
				VkBufferCreateInfo bufferInfo = {};
				bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufferInfo.size = size;
				bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
				bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				VkBuffer buffer = CreateVulkanBuffer(bufferInfo);//
				VkMemoryRequirements bufferMemRequirements = GetBufferMemoryRequirements(buffer);
				VkMemoryAllocateInfo bufferMemoryAllocInfo = {};
				bufferMemoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				bufferMemoryAllocInfo.allocationSize = bufferMemRequirements.size;
				bufferMemoryAllocInfo.memoryTypeIndex = getMemoryTypeIndex(bufferMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

				VkDeviceMemory bufferMem = CreateVulkanDeviceMemory(bufferMemoryAllocInfo);//
				vkBindBufferMemory(getLogicalDevice(), buffer, bufferMem, 0);


				void* bufferMemPointer = nullptr;
				vkMapMemory(getLogicalDevice(), bufferMem, 0, size, 0, &bufferMemPointer);
				memcpy(bufferMemPointer, data, size);
				vkUnmapMemory(getLogicalDevice(), bufferMem);

				VkImageSubresourceRange subresourceRange = {};
				subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				subresourceRange.levelCount = 1;
				subresourceRange.layerCount = 1;
				subresourceRange.baseArrayLayer = 0;
				subresourceRange.baseMipLevel = 0;

				VkCommandBuffer cmdbuf;
				pool.create(1, &cmdbuf);
				makeCmdBegin(cmdbuf);
				makeCmdTransitionImageLayout(cmdbuf, image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);
				makeCmdCopyBufferToImage(cmdbuf, image, buffer, width, height);
				makeCmdTransitionImageLayout(cmdbuf, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, subresourceRange);

				// Copy down mips from n-1 to n
				for (int32_t i = 1; i < mipLevels; i++)
				{
					VkImageBlit imageBlit{};
					// Source
					imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					imageBlit.srcSubresource.layerCount = 1;
					imageBlit.srcSubresource.mipLevel = i - 1;
					imageBlit.srcOffsets[1].x = int32_t(width >> (i - 1));
					imageBlit.srcOffsets[1].y = int32_t(height >> (i - 1)) == 0 ? 1 : int32_t(height >> (i - 1));
					imageBlit.srcOffsets[1].z = int32_t(depth >> (i - 1)) == 0 ? 1 : int32_t(depth >> (i - 1));
					// Destination
					imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					imageBlit.dstSubresource.layerCount = 1;
					imageBlit.dstSubresource.mipLevel = i;
					imageBlit.dstOffsets[1].x = int32_t(width >> i);
					imageBlit.dstOffsets[1].y = int32_t(height >> i) == 0 ? 1 : int32_t(height >> i);
					imageBlit.dstOffsets[1].z = int32_t(depth >> i) == 0 ? 1 : int32_t(depth >> i);
					VkImageSubresourceRange mipSubRange = {};
					mipSubRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					mipSubRange.baseMipLevel = i;
					mipSubRange.levelCount = 1;
					mipSubRange.layerCount = 1;
					// Transiton current mip level to transfer dest
					makeCmdTransitionImageLayout(
						cmdbuf,
						image,
						VK_IMAGE_LAYOUT_UNDEFINED,
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						mipSubRange,
						VK_PIPELINE_STAGE_TRANSFER_BIT,
						VK_PIPELINE_STAGE_TRANSFER_BIT);

					// Blit from previous level
					vkCmdBlitImage(
						cmdbuf,
						image,
						VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						image,
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						1,
						&imageBlit,
						VK_FILTER_LINEAR);

					// Transiton current mip level to transfer source for read in next iteration
					makeCmdTransitionImageLayout(
						cmdbuf,
						image,
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						mipSubRange,
						VK_PIPELINE_STAGE_TRANSFER_BIT,
						VK_PIPELINE_STAGE_TRANSFER_BIT);
				}

				// After the loop, all mip layers are in TRANSFER_SRC layout, so transition all to SHADER_READ
				subresourceRange.levelCount = mipLevels;
				makeCmdTransitionImageLayout(cmdbuf, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);
				vulkan::makeCmdEnd(cmdbuf);
				executeCommands(getTransferQueueIndex(), &cmdbuf, 1);

				pool.destroy(&cmdbuf, 1);
				DestroyMemory(bufferMem);//
				DestroyBuffer(buffer);//
			}

			VkImageViewType image_view_type = depth > 1 ? VK_IMAGE_VIEW_TYPE_3D : (height > 1 ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_1D);

			VkImageView imageView;
			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = image;
			viewInfo.viewType = image_view_type;
			viewInfo.format = (VkFormat)format;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = mipLevels;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;
			imageView = CreateVulkanImageView(viewInfo);//

			ImageTexture package = {};
			package.mImage = image;
			package.mMemory = imageMem;
			package.mView = imageView;
			package.mFormat = (VkFormat)format;
			package.mMipLevel = 1;
			package.mHeight = height;
			package.mWidth = width;
			package.mDepth = depth;

			return package;
		}


		inline ImageTexture LogicalDevice::createTexture1D(const CommandPool& pool, uint32_t width, ImageFormat format, const void* data, uint32_t size, bool mip)
		{
			return createImage(pool, width, 1, 1, format, data, size, mip, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
		}

		inline ImageTexture LogicalDevice::createTexture2D(const CommandPool& pool, uint32_t width, uint32_t height, ImageFormat format, const void* data, uint32_t size, bool mip)
		{
			return createImage(pool, width, height, 1, format, data, size, mip, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
		}

		inline ImageTexture LogicalDevice::createTexture3D(const CommandPool& pool, uint32_t width, uint32_t height, uint32_t depth, ImageFormat format, const void* data, uint32_t size, bool mip)
		{
			return createImage(pool, width, height, depth, format, data, size, mip, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
		}




		ImageTexture LogicalDevice::createStorageImage1D(const CommandPool& pool, uint32_t width, ImageFormat format, const void* data, uint32_t size)
		{
			auto package = createImage(pool, width, 1, 1, format, data, size, false,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
			VkImageSubresourceRange subresourceRange = {};

			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresourceRange.levelCount = 1;
			subresourceRange.layerCount = 1;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.baseMipLevel = 0;
			VkCommandBuffer cmdbuf;
			pool.create(1, &cmdbuf);
			makeCmdBegin(cmdbuf);
			makeCmdTransitionImageLayout(cmdbuf, package.mImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, subresourceRange);
			makeCmdEnd(cmdbuf);
			executeCommands(getTransferQueueIndex(), &cmdbuf, 1);
			pool.destroy(&cmdbuf, 1);
		
			return package;
		}

		ImageTexture LogicalDevice::createStorageImage2D(const CommandPool& pool, uint32_t width, uint32_t height, ImageFormat format, const void* data, uint32_t size)
		{
			auto package = createImage(pool, width, height, 1, format, data, size, false,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
			VkImageSubresourceRange subresourceRange = {};

			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresourceRange.levelCount = 1;
			subresourceRange.layerCount = 1;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.baseMipLevel = 0;
			VkCommandBuffer cmdbuf;
			pool.create(1, &cmdbuf);
			makeCmdBegin(cmdbuf);
			makeCmdTransitionImageLayout(cmdbuf, package.mImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, subresourceRange);
			makeCmdEnd(cmdbuf);
			executeCommands(getTransferQueueIndex(), &cmdbuf, 1);
			pool.destroy(&cmdbuf, 1);

			return package;
		}

		ImageTexture LogicalDevice::createStorageImage3D(const CommandPool& pool, uint32_t width, uint32_t height, uint32_t depth, ImageFormat format, const void* data, uint32_t size)
		{
			auto package = createImage(pool, width, height, depth, format, data, size, false,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
			VkImageSubresourceRange subresourceRange = {};

			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresourceRange.levelCount = 1;
			subresourceRange.layerCount = 1;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.baseMipLevel = 0;
			VkCommandBuffer cmdbuf;
			pool.create(1, &cmdbuf);
			makeCmdBegin(cmdbuf);
			makeCmdTransitionImageLayout(cmdbuf, package.mImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, subresourceRange);
			makeCmdEnd(cmdbuf);
			executeCommands(getTransferQueueIndex(), &cmdbuf, 1);
			pool.destroy(&cmdbuf, 1);

			return package;
		}



		inline void LogicalDevice::updateImage2D(const CommandPool& pool, ImageTexture* pImage, uint32_t width, uint32_t height, const void* data, uint32_t size, ImageFormat format)
		{
			assert(pImage->mWidth == width);
			assert(pImage->mHeight == height);
			assert(pImage->mFormat == (VkFormat)format);
			VkBufferCreateInfo bufferInfo = {};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VkBuffer buffer = CreateVulkanBuffer(bufferInfo);//
			VkMemoryRequirements bufferMemRequirements = GetBufferMemoryRequirements(buffer);
			VkMemoryAllocateInfo bufferMemoryAllocInfo = {};
			bufferMemoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			bufferMemoryAllocInfo.allocationSize = bufferMemRequirements.size;
			bufferMemoryAllocInfo.memoryTypeIndex = getMemoryTypeIndex(bufferMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			VkDeviceMemory bufferMem = CreateVulkanDeviceMemory(bufferMemoryAllocInfo);//

			vkBindBufferMemory(getLogicalDevice(), buffer, bufferMem, 0);

			void* bufferMemPointer = nullptr;
			vkMapMemory(getLogicalDevice(), bufferMem, 0, size, 0, &bufferMemPointer);
			memcpy(bufferMemPointer, data, size);
			vkUnmapMemory(getLogicalDevice(), bufferMem);

			VkImageSubresourceRange subresourceRange = {};
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresourceRange.levelCount = 1;
			subresourceRange.layerCount = 1;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.baseMipLevel = 0;

			VkCommandBuffer cmdbuf;
			pool.create(1, &cmdbuf);
			makeCmdBegin(cmdbuf);
			makeCmdTransitionImageLayout(cmdbuf, pImage->mImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);
			makeCmdCopyBufferToImage(cmdbuf, pImage->mImage, buffer, width, height);
			makeCmdTransitionImageLayout(cmdbuf, pImage->mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, subresourceRange);

			// Copy down mips from n-1 to n
			for (int32_t i = 1; i < pImage->mMipLevel; i++)
			{
				VkImageBlit imageBlit{};
				// Source
				imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageBlit.srcSubresource.layerCount = 1;
				imageBlit.srcSubresource.mipLevel = i - 1;
				imageBlit.srcOffsets[1].x = int32_t(width >> (i - 1));
				imageBlit.srcOffsets[1].y = int32_t(height >> (i - 1));
				imageBlit.srcOffsets[1].z = 1;//
											  // Destination
				imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageBlit.dstSubresource.layerCount = 1;
				imageBlit.dstSubresource.mipLevel = i;
				imageBlit.dstOffsets[1].x = int32_t(width >> i);
				imageBlit.dstOffsets[1].y = int32_t(height >> i);
				imageBlit.dstOffsets[1].z = 1;
				VkImageSubresourceRange mipSubRange = {};
				mipSubRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				mipSubRange.baseMipLevel = i;
				mipSubRange.levelCount = 1;
				mipSubRange.layerCount = 1;
				// Transiton current mip level to transfer dest
				makeCmdTransitionImageLayout(
					cmdbuf,
					pImage->mImage,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					mipSubRange,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_PIPELINE_STAGE_TRANSFER_BIT);

				// Blit from previous level
				vkCmdBlitImage(
					cmdbuf,
					pImage->mImage,
					VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					pImage->mImage,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1,
					&imageBlit,
					VK_FILTER_LINEAR);

				// Transiton current mip level to transfer source for read in next iteration
				makeCmdTransitionImageLayout(
					cmdbuf,
					pImage->mImage,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					mipSubRange,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_PIPELINE_STAGE_TRANSFER_BIT);
			}

			// After the loop, all mip layers are in TRANSFER_SRC layout, so transition all to SHADER_READ
			subresourceRange.levelCount = pImage->mMipLevel;
			makeCmdTransitionImageLayout(cmdbuf, pImage->mImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);
			vulkan::makeCmdEnd(cmdbuf);
			executeCommands(getTransferQueueIndex(), &cmdbuf, 1);
			pool.destroy(&cmdbuf, 1);
			DestroyMemory(bufferMem);//
			DestroyBuffer(buffer);//
		}



		inline BufferMemory LogicalDevice::createBuffer(const CommandPool& pool, uint32_t size, VkBufferUsageFlags usages, VkMemoryPropertyFlags mem_property, const void* data)
		{
			int aligned_size = size == 0 ? 64 : size;

			VkBufferCreateInfo bufferInfo = {};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = aligned_size;// 
			bufferInfo.usage = usages; // VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;//
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			VkBuffer buffer = CreateVulkanBuffer(bufferInfo);//
			VkMemoryRequirements vertexBufferMemRequirements = GetBufferMemoryRequirements(buffer);
			VkMemoryAllocateInfo vertexBufferMemoryAllocInfo = {};
			vertexBufferMemoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			vertexBufferMemoryAllocInfo.allocationSize = vertexBufferMemRequirements.size;
			vertexBufferMemoryAllocInfo.memoryTypeIndex = getMemoryTypeIndex(vertexBufferMemRequirements.memoryTypeBits, mem_property);
			VkDeviceMemory bufferMem = CreateVulkanDeviceMemory(vertexBufferMemoryAllocInfo);//
			vkBindBufferMemory(getLogicalDevice(), buffer, bufferMem, 0);
			void* bufferMemPointer = nullptr;

			if (mem_property & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
			{
				vkMapMemory(getLogicalDevice(), bufferMem, 0, aligned_size, 0, &bufferMemPointer);
			}

			if (data)
			{
				if (mem_property & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
				{
					VkBufferCreateInfo stagingBufferInfo = {};
					stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
					stagingBufferInfo.size = aligned_size;
					stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
					stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
					VkBuffer stagingBuffer = CreateVulkanBuffer(stagingBufferInfo);//
					VkMemoryRequirements stagingBufferMemRequirements = GetBufferMemoryRequirements(stagingBuffer);
					VkMemoryAllocateInfo stagingBufferMemoryAllocInfo = {};
					stagingBufferMemoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
					stagingBufferMemoryAllocInfo.allocationSize = stagingBufferMemRequirements.size;
					stagingBufferMemoryAllocInfo.memoryTypeIndex = getMemoryTypeIndex(stagingBufferMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
					VkDeviceMemory stagingBufferMem = CreateVulkanDeviceMemory(stagingBufferMemoryAllocInfo);//
					vkBindBufferMemory(getLogicalDevice(), stagingBuffer, stagingBufferMem, 0);
					void* stagingBufferMemPointer = nullptr;
					vkMapMemory(getLogicalDevice(), stagingBufferMem, 0, aligned_size, 0, &stagingBufferMemPointer);
					memcpy(stagingBufferMemPointer, data, size);
					vkUnmapMemory(getLogicalDevice(), stagingBufferMem);
					VkCommandBuffer cmdbuf;
					pool.create(1, &cmdbuf);
					vulkan::makeCmdBegin(cmdbuf);
					makeCmdCopyBufferToBuffer(cmdbuf, buffer, stagingBuffer, size);
					vulkan::makeCmdEnd(cmdbuf);
					executeCommands(getTransferQueueIndex(), &cmdbuf, 1); //
					pool.destroy(&cmdbuf, 1);
					DestroyMemory(stagingBufferMem);//
					DestroyBuffer(stagingBuffer);//
				}
				else if (mem_property & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
				{
					memcpy(bufferMemPointer, data, size);
				}
				else
				{

				}
			}

			BufferMemory package;
			package.mBuffer = buffer;
			package.mMemory = bufferMem;
			package.mAddress = bufferMemPointer;
			package.mSize = aligned_size;
			return package;
		}


		inline void LogicalDevice::copyDataToBuffer(const CommandPool& pool, VkBuffer dst, const void* src, uint32_t size, uint32_t offset)
		{
			VkBufferCreateInfo stagingBufferInfo = {};
			stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			stagingBufferInfo.size = size;
			stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VkBuffer stagingBuffer = CreateVulkanBuffer(stagingBufferInfo);//
			VkMemoryRequirements stagingBufferMemRequirements = GetBufferMemoryRequirements(stagingBuffer);

			VkMemoryAllocateInfo stagingBufferMemoryAllocInfo = {};
			stagingBufferMemoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			stagingBufferMemoryAllocInfo.allocationSize = stagingBufferMemRequirements.size;
			stagingBufferMemoryAllocInfo.memoryTypeIndex = getMemoryTypeIndex(stagingBufferMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			VkDeviceMemory stagingBufferMem = CreateVulkanDeviceMemory(stagingBufferMemoryAllocInfo);//
			vkBindBufferMemory(getLogicalDevice(), stagingBuffer, stagingBufferMem, 0);

			void* bufferMemPointer = nullptr;
			vkMapMemory(getLogicalDevice(), stagingBufferMem, offset, size, 0, &bufferMemPointer);
			memcpy(bufferMemPointer, (void*)((char*)src + offset), size);
			vkUnmapMemory(getLogicalDevice(), stagingBufferMem);

			VkCommandBuffer cmdbuf;
			pool.create(1, &cmdbuf);
			vulkan::makeCmdBegin(cmdbuf);
			makeCmdCopyBufferToBuffer(cmdbuf, dst, stagingBuffer, size);
			vulkan::makeCmdEnd(cmdbuf);
			executeCommands(getTransferQueueIndex(), &cmdbuf, 1); //
			pool.destroy(&cmdbuf, 1);

			DestroyMemory(stagingBufferMem);//
			DestroyBuffer(stagingBuffer);//
		}

		inline void LogicalDevice::copyBufferToData(const CommandPool& pool, void* dst, VkBuffer src, uint32_t size, uint32_t offset)
		{
			VkBufferCreateInfo stagingBufferInfo = {};
			stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			stagingBufferInfo.size = size;
			stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VkBuffer stagingBuffer = CreateVulkanBuffer(stagingBufferInfo);//

			VkMemoryRequirements stagingBufferMemRequirements = GetBufferMemoryRequirements(stagingBuffer);

			VkMemoryAllocateInfo stagingBufferMemoryAllocInfo = {};
			stagingBufferMemoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			stagingBufferMemoryAllocInfo.allocationSize = stagingBufferMemRequirements.size;
			stagingBufferMemoryAllocInfo.memoryTypeIndex = getMemoryTypeIndex(stagingBufferMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			VkDeviceMemory stagingBufferMem = CreateVulkanDeviceMemory(stagingBufferMemoryAllocInfo);//
			vkBindBufferMemory(getLogicalDevice(), stagingBuffer, stagingBufferMem, 0);

			VkCommandBuffer cmdbuf;
			pool.create(1, &cmdbuf);
			vulkan::makeCmdBegin(cmdbuf);
			makeCmdCopyBufferToBuffer(cmdbuf, stagingBuffer, src, size);
			vulkan::makeCmdEnd(cmdbuf);
			executeCommands(getTransferQueueIndex(), &cmdbuf, 1); //

			void* bufferMemPointer = nullptr;
			vkMapMemory(getLogicalDevice(), stagingBufferMem, 0, size, 0, &bufferMemPointer);
			memcpy(dst, (void*)((char*)bufferMemPointer + offset), size);
			vkUnmapMemory(getLogicalDevice(), stagingBufferMem);

			pool.destroy(&cmdbuf, 1);
			DestroyMemory(stagingBufferMem);//
			DestroyBuffer(stagingBuffer);//
		}

		inline void LogicalDevice::copyBufferToBuffer(const CommandPool& pool, VkBuffer dst, VkBuffer src, uint32_t size, uint32_t dst_offset, uint32_t src_offset)
		{
			VkCommandBuffer cmdbuf;
			pool.create(1, &cmdbuf);
			vulkan::makeCmdBegin(cmdbuf);
			makeCmdCopyBufferToBuffer(cmdbuf, dst, src, size, dst_offset, src_offset);
			vulkan::makeCmdEnd(cmdbuf);
			executeCommands(getTransferQueueIndex(), &cmdbuf, 1); //
			pool.destroy(&cmdbuf, 1);
		}

		inline void LogicalDevice::copyImageToBuffer(const CommandPool& pool, VkBuffer dst, VkImage src, uint32_t width, uint32_t height, uint32_t depth)
		{
			VkCommandBuffer cmdbuf;
			pool.create(1, &cmdbuf);
			vulkan::makeCmdBegin(cmdbuf);
			makeCmdCopyImageToBuffer(cmdbuf, dst, src, width, height, depth);
			vulkan::makeCmdEnd(cmdbuf);
			executeCommands(getTransferQueueIndex(), &cmdbuf, 1); //
			pool.destroy(&cmdbuf, 1);
		}

		inline void LogicalDevice::destroyFramebuffer(VkFramebuffer framebuffer) const
		{
			vkDestroyFramebuffer(m_logicalDevice, framebuffer, nullptr);
		}

		inline void LogicalDevice::destroyShaderModule(VkShaderModule shaderModule) const
		{
			vkDestroyShaderModule(m_logicalDevice, shaderModule, nullptr);
		}

		inline void LogicalDevice::destroySemaphore(VkSemaphore semaphore) const
		{
			vkDestroySemaphore(m_logicalDevice, semaphore, nullptr);
		}

		inline void LogicalDevice::destroySampler(VkSampler sampler)const { vkDestroySampler(m_logicalDevice, sampler, nullptr); }
		inline void LogicalDevice::destroyRenderPass(VkRenderPass renderPass)const { vkDestroyRenderPass(m_logicalDevice, renderPass, nullptr); }

		inline void LogicalDevice::destroyPipeline(VkPipeline pipeline)const { vkDestroyPipeline(m_logicalDevice, pipeline, nullptr); }
		inline void LogicalDevice::destroyPipelineLayout(VkPipelineLayout pipelineLayout)const { vkDestroyPipelineLayout(m_logicalDevice, pipelineLayout, nullptr); }
		inline void LogicalDevice::destroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout)const { vkDestroyDescriptorSetLayout(m_logicalDevice, descriptorSetLayout, nullptr); }

		inline void LogicalDevice::destroy(CommandPool commandPool)const { vkDestroyCommandPool(m_logicalDevice, commandPool.mVulkanCommandPoolObject, nullptr); }
		inline void LogicalDevice::destroy(DescriptorPool descriptorPool)const { vkDestroyDescriptorPool(m_logicalDevice, descriptorPool.mVulkanDescriptorPoolObject, nullptr); }
		inline void LogicalDevice::destroy(ImageTexture image)const
		{
			this->DestroyImageView(image.mView);
			if (image.mStencilView != VK_NULL_HANDLE)
				this->DestroyImageView(image.mStencilView);
			if (image.mDepthView != VK_NULL_HANDLE)
				this->DestroyImageView(image.mDepthView);
			this->DestroyMemory(image.mMemory);
			this->DestroyImage(image.mImage);
		}

		inline void LogicalDevice::destroy(BufferMemory buffer)const
		{
			if (buffer.mAddress != nullptr)
				vkUnmapMemory(m_logicalDevice, buffer.mMemory);
			DestroyMemory(buffer.mMemory);
			DestroyBuffer(buffer.mBuffer);
		}



	}


}



#endif // !CRAFT_ENGINE_VYLKAN_API_H_