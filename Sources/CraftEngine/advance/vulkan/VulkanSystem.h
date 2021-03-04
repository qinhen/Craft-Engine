#pragma once
#ifndef CRAFT_ENGINE_VULKAN_SYSTEM_H_
#define CRAFT_ENGINE_VULKAN_SYSTEM_H_

#include <unordered_set>
#include <string>

#include "./config.inl"
#include "./VulkanHighLevel.h"

namespace CraftEngine
{
	namespace vulkan
	{


		class VulkanSystem
		{
		private:
			static Instance* m_vkInstance;
			static PhysicalDevice* m_vkPhysicalDevice;
			static LogicalDevice* m_vkLogicalDevece;
			static bool m_initilized;

			static std::unordered_set<std::string> m_instanceLayers;
			static std::unordered_set<std::string> m_instanceExtensions;
			static std::unordered_set<std::string> m_logicalDeviceExtensions;
			static int m_ext;
		public:

			enum
			{
				eExtDebugReport = 0x1,
				eExtSurface = 0x2,
				eExtRayTracing = 0x4,
			};

			static bool isInitialized()
			{
				return m_initilized;
			}

			static void initSystem()
			{
				if (!m_initilized)
				{
					if (m_ext & eExtDebugReport)
					{
						m_instanceLayers.emplace("VK_LAYER_KHRONOS_validation"); // VK_LAYER_KHRONOS_validation VK_LAYER_LUNARG_standard_validation
						m_instanceExtensions.emplace(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
					}
					if (m_ext & eExtSurface)
					{
						m_instanceExtensions.emplace("VK_KHR_surface");
						m_instanceExtensions.emplace("VK_KHR_win32_surface");
						m_logicalDeviceExtensions.emplace(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
					}

					if (VK_NV_framebuffer_mixed_samples)
					{
						m_logicalDeviceExtensions.emplace(VK_NV_FRAMEBUFFER_MIXED_SAMPLES_EXTENSION_NAME);
					}
					else if (VK_AMD_mixed_attachment_samples)
					{
						m_logicalDeviceExtensions.emplace(VK_AMD_MIXED_ATTACHMENT_SAMPLES_EXTENSION_NAME);
					}
					
					//m_logicalDeviceExtensions.emplace(VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME);		
					//m_instanceExtensions.emplace(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
					
					if (m_ext & eExtRayTracing)
					{
						m_instanceExtensions.emplace(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
						m_logicalDeviceExtensions.emplace(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
						m_logicalDeviceExtensions.emplace(VK_NV_RAY_TRACING_EXTENSION_NAME);
					}

					std::vector<const char*> instanceLayers;
					std::vector<const char*> instanceExtensions;
					std::vector<const char*> logicalDeviceExtensions;

					for (auto& it : m_instanceLayers)
						instanceLayers.push_back(it.c_str());
					for (auto& it : m_instanceExtensions)
						instanceExtensions.push_back(it.c_str());
					for (auto& it : m_logicalDeviceExtensions)
						logicalDeviceExtensions.push_back(it.c_str());

					m_vkInstance = new Instance("Craft Engine Application", "Craft Engine", instanceLayers, instanceExtensions);

					if (m_ext & eExtDebugReport)
						m_vkInstance->enableDebugReport();

					m_vkPhysicalDevice = new PhysicalDevice(m_vkInstance->getBestPhysicalDeviceIndex(), m_vkInstance);

					m_vkLogicalDevece = new LogicalDevice(CFEG_VULKAN_MAX_QUEUE_COUNT, instanceLayers, logicalDeviceExtensions, { }, m_vkPhysicalDevice);

					if (m_ext & eExtSurface)
					{
						//m_vkInstance->getInstanceProcAddr("vkGetPhysicalDeviceSurfaceSupportKHR");
						//m_vkInstance->getInstanceProcAddr("vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
						//m_vkInstance->getInstanceProcAddr("vkGetPhysicalDeviceSurfaceFormatsKHR");
						//m_vkInstance->getInstanceProcAddr("vkGetPhysicalDeviceSurfacePresentModesKHR");
						//m_vkLogicalDevece->getDeviceProcAddr("vkCreateSwapchainKHR");
						//m_vkLogicalDevece->getDeviceProcAddr("vkDestroySwapchainKHR");
						//m_vkLogicalDevece->getDeviceProcAddr("vkGetSwapchainImagesKHR");
						//m_vkLogicalDevece->getDeviceProcAddr("vkAcquireNextImageKHR");
						//m_vkLogicalDevece->getDeviceProcAddr("vkQueuePresentKHR");
					}
					
					//_Detail::cmdSetSampleLocationsEXT = reinterpret_cast<PFN_vkCmdSetSampleLocationsEXT>(m_vkLogicalDevece->getDeviceProcAddr("vkCmdSetSampleLocationsEXT"));
					if (m_ext & eExtRayTracing)
					{
						std::cout << "eExtRayTracing" << std::endl;

						_Detail::createAccelerationStructureNV = reinterpret_cast<PFN_vkCreateAccelerationStructureNV>(m_vkLogicalDevece->getDeviceProcAddr("vkCreateAccelerationStructureNV"));
						_Detail::destroyAccelerationStructureNV = reinterpret_cast<PFN_vkDestroyAccelerationStructureNV>(m_vkLogicalDevece->getDeviceProcAddr("vkDestroyAccelerationStructureNV"));
						_Detail::bindAccelerationStructureMemoryNV = reinterpret_cast<PFN_vkBindAccelerationStructureMemoryNV>(m_vkLogicalDevece->getDeviceProcAddr("vkBindAccelerationStructureMemoryNV"));
						_Detail::getAccelerationStructureHandleNV = reinterpret_cast<PFN_vkGetAccelerationStructureHandleNV>(m_vkLogicalDevece->getDeviceProcAddr("vkGetAccelerationStructureHandleNV"));
						_Detail::getAccelerationStructureMemoryRequirementsNV = reinterpret_cast<PFN_vkGetAccelerationStructureMemoryRequirementsNV>(m_vkLogicalDevece->getDeviceProcAddr("vkGetAccelerationStructureMemoryRequirementsNV"));
						_Detail::cmdBuildAccelerationStructureNV = reinterpret_cast<PFN_vkCmdBuildAccelerationStructureNV>(m_vkLogicalDevece->getDeviceProcAddr("vkCmdBuildAccelerationStructureNV"));
						_Detail::createRayTracingPipelinesNV = reinterpret_cast<PFN_vkCreateRayTracingPipelinesNV>(m_vkLogicalDevece->getDeviceProcAddr("vkCreateRayTracingPipelinesNV"));
						_Detail::getRayTracingShaderGroupHandlesNV = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesNV>(m_vkLogicalDevece->getDeviceProcAddr("vkGetRayTracingShaderGroupHandlesNV"));
						_Detail::cmdTraceRaysNV = reinterpret_cast<PFN_vkCmdTraceRaysNV>(m_vkLogicalDevece->getDeviceProcAddr("vkCmdTraceRaysNV"));
						_Detail::cmdCopyAccelerationStructureNV = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureNV>(m_vkLogicalDevece->getDeviceProcAddr("vkCmdCopyAccelerationStructureNV"));

					}
					m_initilized = true;
					std::cout << "Vulkan System is initialized." << std::endl;
				}
				else
					throw std::runtime_error("");

			}


			static void destroySystem()
			{
				if (m_initilized)
				{
					delete m_vkLogicalDevece;
					delete m_vkPhysicalDevice;
					delete m_vkInstance;
					m_initilized = false;
					std::cout << "Vulkan System is destroyed." << std::endl;
				}
				else
				{
					throw std::runtime_error("");
				}
			}

			static Instance* getInstance()
			{
				return m_vkInstance;
			}

			static PhysicalDevice* getPhysicalDevice()
			{
				return m_vkPhysicalDevice;
			}

			static LogicalDevice* getLogicalDevice()
			{
				return m_vkLogicalDevece;
			}

			static void enable(int e)
			{
				if(e & eExtDebugReport)
					m_ext |= eExtDebugReport;
				if (e & eExtSurface)
					m_ext |= eExtSurface;
				if (e & eExtRayTracing)
					m_ext |= eExtRayTracing;

			}
		};

		Instance* VulkanSystem::m_vkInstance = nullptr;
		PhysicalDevice* VulkanSystem::m_vkPhysicalDevice = nullptr;
		LogicalDevice* VulkanSystem::m_vkLogicalDevece = nullptr;
		bool VulkanSystem::m_initilized = false;
		std::unordered_set<std::string> VulkanSystem::m_instanceLayers;
		std::unordered_set<std::string> VulkanSystem::m_instanceExtensions;
		std::unordered_set<std::string> VulkanSystem::m_logicalDeviceExtensions;
		int VulkanSystem::m_ext = eExtSurface;

	}




	 



}
#endif