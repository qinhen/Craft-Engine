#pragma once
#ifndef CRAFT_ENGINE_VYLKAN_API_H_
#define CRAFT_ENGINE_VYLKAN_API_H_



#include "../../3rdparty/ThirdPartyImportVulkan.h"



namespace CraftEngine
{


	namespace vulkan
	{



		namespace _Detail
		{
			// nv ray tracking api
			PFN_vkCreateAccelerationStructureNV createAccelerationStructureNV;
			PFN_vkDestroyAccelerationStructureNV destroyAccelerationStructureNV;
			PFN_vkBindAccelerationStructureMemoryNV bindAccelerationStructureMemoryNV;
			PFN_vkGetAccelerationStructureHandleNV getAccelerationStructureHandleNV;
			PFN_vkGetAccelerationStructureMemoryRequirementsNV getAccelerationStructureMemoryRequirementsNV;
			PFN_vkCmdBuildAccelerationStructureNV cmdBuildAccelerationStructureNV;
			PFN_vkCreateRayTracingPipelinesNV createRayTracingPipelinesNV;
			PFN_vkGetRayTracingShaderGroupHandlesNV getRayTracingShaderGroupHandlesNV;
			PFN_vkCmdTraceRaysNV cmdTraceRaysNV;
			PFN_vkCmdCopyAccelerationStructureNV cmdCopyAccelerationStructureNV;

			PFN_vkCmdSetSampleLocationsEXT cmdSetSampleLocationsEXT;
		}

		namespace api
		{


			VKAPI_ATTR VkResult VKAPI_CALL createAccelerationStructureNV(
				VkDevice                                    device,
				const VkAccelerationStructureCreateInfoNV* pCreateInfo,
				const VkAllocationCallbacks* pAllocator,
				VkAccelerationStructureNV* pAccelerationStructure)
			{
				return _Detail::createAccelerationStructureNV(device, pCreateInfo, pAllocator, pAccelerationStructure);
			}

			VKAPI_ATTR void VKAPI_CALL destroyAccelerationStructureNV(
				VkDevice                                    device,
				VkAccelerationStructureNV                   accelerationStructure,
				const VkAllocationCallbacks* pAllocator)
			{
				_Detail::destroyAccelerationStructureNV(device, accelerationStructure, pAllocator);
			}

			VKAPI_ATTR void VKAPI_CALL getAccelerationStructureMemoryRequirementsNV(
				VkDevice                                    device,
				const VkAccelerationStructureMemoryRequirementsInfoNV* pInfo,
				VkMemoryRequirements2KHR* pMemoryRequirements)
			{
				_Detail::getAccelerationStructureMemoryRequirementsNV(device, pInfo, pMemoryRequirements);
			}

			VKAPI_ATTR VkResult VKAPI_CALL bindAccelerationStructureMemoryNV(
				VkDevice                                    device,
				uint32_t                                    bindInfoCount,
				const VkBindAccelerationStructureMemoryInfoNV* pBindInfos)
			{
				return _Detail::bindAccelerationStructureMemoryNV(device, bindInfoCount, pBindInfos);
			}

			VKAPI_ATTR void VKAPI_CALL cmdBuildAccelerationStructureNV(
				VkCommandBuffer                             commandBuffer,
				const VkAccelerationStructureInfoNV* pInfo,
				VkBuffer                                    instanceData,
				VkDeviceSize                                instanceOffset,
				VkBool32                                    update,
				VkAccelerationStructureNV                   dst,
				VkAccelerationStructureNV                   src,
				VkBuffer                                    scratch,
				VkDeviceSize                                scratchOffset)
			{
				_Detail::cmdBuildAccelerationStructureNV(commandBuffer, pInfo, instanceData, instanceOffset, update, dst, src, scratch, scratchOffset);
			}

			VKAPI_ATTR void VKAPI_CALL cmdCopyAccelerationStructureNV(
				VkCommandBuffer                             commandBuffer,
				VkAccelerationStructureNV                   dst,
				VkAccelerationStructureNV                   src,
				VkCopyAccelerationStructureModeNV           mode)
			{
				_Detail::cmdCopyAccelerationStructureNV(commandBuffer, dst, src, mode);
			}

			VKAPI_ATTR void VKAPI_CALL cmdTraceRaysNV(
				VkCommandBuffer                             commandBuffer,
				VkBuffer                                    raygenShaderBindingTableBuffer,
				VkDeviceSize                                raygenShaderBindingOffset,
				VkBuffer                                    missShaderBindingTableBuffer,
				VkDeviceSize                                missShaderBindingOffset,
				VkDeviceSize                                missShaderBindingStride,
				VkBuffer                                    hitShaderBindingTableBuffer,
				VkDeviceSize                                hitShaderBindingOffset,
				VkDeviceSize                                hitShaderBindingStride,
				VkBuffer                                    x,
				VkDeviceSize                                callableShaderBindingOffset,
				VkDeviceSize                                callableShaderBindingStride,
				uint32_t                                    width,
				uint32_t                                    height,
				uint32_t                                    depth)
			{
				_Detail::cmdTraceRaysNV(commandBuffer, raygenShaderBindingTableBuffer, raygenShaderBindingOffset,
					missShaderBindingTableBuffer, missShaderBindingOffset, missShaderBindingStride,
					hitShaderBindingTableBuffer, hitShaderBindingOffset, hitShaderBindingStride,
					x, callableShaderBindingOffset, callableShaderBindingStride,
					width, height, depth
				);
			}

			VKAPI_ATTR VkResult VKAPI_CALL createRayTracingPipelinesNV(
				VkDevice                                    device,
				VkPipelineCache                             pipelineCache,
				uint32_t                                    createInfoCount,
				const VkRayTracingPipelineCreateInfoNV* pCreateInfos,
				const VkAllocationCallbacks* pAllocator,
				VkPipeline* pPipelines)
			{
				return _Detail::createRayTracingPipelinesNV(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
			}

			VKAPI_ATTR VkResult VKAPI_CALL getRayTracingShaderGroupHandlesNV(
				VkDevice                                    device,
				VkPipeline                                  pipeline,
				uint32_t                                    firstGroup,
				uint32_t                                    groupCount,
				size_t                                      dataSize,
				void* pData)
			{
				return _Detail::getRayTracingShaderGroupHandlesNV(device, pipeline, firstGroup, groupCount, dataSize, pData);
			}

			VKAPI_ATTR VkResult VKAPI_CALL getAccelerationStructureHandleNV(
				VkDevice                                    device,
				VkAccelerationStructureNV                   accelerationStructure,
				size_t                                      dataSize,
				void* pData)
			{
				return _Detail::getAccelerationStructureHandleNV(device, accelerationStructure, dataSize, pData);
			}

			//VKAPI_ATTR void VKAPI_CALL cmdWriteAccelerationStructuresPropertiesNV(
			//	VkCommandBuffer                             commandBuffer,
			//	uint32_t                                    accelerationStructureCount,
			//	const VkAccelerationStructureNV* pAccelerationStructures,
			//	VkQueryType                                 queryType,
			//	VkQueryPool                                 queryPool,
			//	uint32_t                                    firstQuery)
			//{
			//	_Detail::cmdWriteAccelerationStructuresPropertiesNV(device, pipeline, firstGroup, groupCount, dataSize, pData);
			//}

			//VKAPI_ATTR VkResult VKAPI_CALL vkCompileDeferredNV(
			//	VkDevice                                    device,
			//	VkPipeline                                  pipeline,
			//	uint32_t                                    shader);


			VKAPI_ATTR void VKAPI_CALL cmdSetSampleLocationsEXT(
				VkCommandBuffer                             commandBuffer,
				const VkSampleLocationsInfoEXT* pSampleLocationsInfo) {
				_Detail::cmdSetSampleLocationsEXT(commandBuffer, pSampleLocationsInfo);
			}

		}


	}


}



#endif // !CRAFT_ENGINE_VYLKAN_API_H_