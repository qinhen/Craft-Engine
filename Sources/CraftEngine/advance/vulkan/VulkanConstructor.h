#pragma once
#ifndef CRAFT_ENGINE_VULKAN_CONSTRUCTOR_H_
#define CRAFT_ENGINE_VULKAN_CONSTRUCTOR_H_



#include "../../3rdparty/ThirdPartyImportVulkan.h"



namespace CraftEngine
{


	namespace vulkan
	{

		namespace vkt
		{


			static VkAttachmentDescription attachmentDescription(
				VkAttachmentDescriptionFlags    flags,
				VkFormat                        format,
				VkSampleCountFlagBits           samples,
				VkAttachmentLoadOp              loadOp,
				VkAttachmentStoreOp             storeOp,
				VkAttachmentLoadOp              stencilLoadOp,
				VkAttachmentStoreOp             stencilStoreOp,
				VkImageLayout                   initialLayout,
				VkImageLayout                   finalLayout
			) {
				return { flags, format, samples, loadOp, storeOp, stencilLoadOp, stencilStoreOp, initialLayout, finalLayout };
			}

			static VkAttachmentReference attachmentReference(
				uint32_t         attachment,
				VkImageLayout    layout
			) {
				VkAttachmentReference attachmentReference = {};
				attachmentReference.attachment = attachment;
				attachmentReference.layout = layout;
				return attachmentReference;
			}

			static VkDescriptorBufferInfo descriptorBufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
			{
				VkDescriptorBufferInfo bufferInfo;
				bufferInfo.buffer = buffer;
				bufferInfo.offset = 0;
				bufferInfo.range = range;
				return bufferInfo;
			}
			static VkDescriptorImageInfo descriptorImageInfo(VkSampler sampler, VkImageView image, VkImageLayout layout)
			{
				VkDescriptorImageInfo imageInfo;
				imageInfo.imageLayout = layout;
				imageInfo.imageView = image;
				imageInfo.sampler = sampler;
				return imageInfo;
			}

			static VkDescriptorPoolSize descriptorPoolSize(
				VkDescriptorType descriptorType,
				uint32_t descriptorCount
			) {
				VkDescriptorPoolSize pipelineColorBlendAttachmentState{};
				pipelineColorBlendAttachmentState.type = descriptorType;
				pipelineColorBlendAttachmentState.descriptorCount = descriptorCount;
				return pipelineColorBlendAttachmentState;
			}

			static VkDescriptorSetLayoutBinding descriptorSetLayoutBinding(
				uint32_t              binding,
				VkDescriptorType      descriptorType,
				uint32_t              descriptorCount,
				VkShaderStageFlags    stageFlags,
				const VkSampler*      pImmutableSamplers = nullptr
			) {
				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
				descriptorSetLayoutBinding.binding = binding;
				descriptorSetLayoutBinding.descriptorType = descriptorType;
				descriptorSetLayoutBinding.descriptorCount = descriptorCount;
				descriptorSetLayoutBinding.stageFlags = stageFlags;
				descriptorSetLayoutBinding.pImmutableSamplers = pImmutableSamplers;
				return descriptorSetLayoutBinding;
			}
			static VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo(
				//VkPipelineCreateFlags                            flags,
				uint32_t                                         stageCount,
				const VkPipelineShaderStageCreateInfo* pStages,
				const VkPipelineVertexInputStateCreateInfo* pVertexInputState,
				const VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyState,
				const VkPipelineTessellationStateCreateInfo* pTessellationState,
				const VkPipelineViewportStateCreateInfo* pViewportState,
				const VkPipelineRasterizationStateCreateInfo* pRasterizationState,
				const VkPipelineMultisampleStateCreateInfo* pMultisampleState,
				const VkPipelineDepthStencilStateCreateInfo* pDepthStencilState,
				const VkPipelineColorBlendStateCreateInfo* pColorBlendState,
				const VkPipelineDynamicStateCreateInfo* pDynamicState,
				VkPipelineLayout                                 layout,
				VkRenderPass                                     renderPass,
				uint32_t                                         subpass,
				VkPipeline                                       basePipelineHandle,
				int32_t                                          basePipelineIndex
			) {
				VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
				graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				graphicsPipelineCreateInfo.pNext = nullptr;
				//graphicsPipelineCreateInfo.flags = flags;
				graphicsPipelineCreateInfo.stageCount = stageCount;
				graphicsPipelineCreateInfo.pStages = pStages;
				graphicsPipelineCreateInfo.pVertexInputState = pVertexInputState;
				graphicsPipelineCreateInfo.pInputAssemblyState = pInputAssemblyState;
				graphicsPipelineCreateInfo.pTessellationState = pTessellationState;
				graphicsPipelineCreateInfo.pViewportState = pViewportState;
				graphicsPipelineCreateInfo.pRasterizationState = pRasterizationState;
				graphicsPipelineCreateInfo.pMultisampleState = pMultisampleState;
				graphicsPipelineCreateInfo.pDepthStencilState = pDepthStencilState;
				graphicsPipelineCreateInfo.pColorBlendState = pColorBlendState;
				graphicsPipelineCreateInfo.pDynamicState = pDynamicState;
				graphicsPipelineCreateInfo.layout = layout;
				graphicsPipelineCreateInfo.renderPass = renderPass;
				graphicsPipelineCreateInfo.subpass = subpass;
				graphicsPipelineCreateInfo.basePipelineHandle = basePipelineHandle;
				graphicsPipelineCreateInfo.basePipelineIndex = basePipelineIndex;
				return graphicsPipelineCreateInfo;
			}
			
			static inline VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState(
				VkBool32                 blendEnable,
				VkBlendFactor            srcColorBlendFactor,
				VkBlendFactor            dstColorBlendFactor,
				VkBlendOp                colorBlendOp,
				VkBlendFactor            srcAlphaBlendFactor,
				VkBlendFactor            dstAlphaBlendFactor,
				VkBlendOp                alphaBlendOp,
				VkColorComponentFlags    colorWriteMask
			){
				VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};
				pipelineColorBlendAttachmentState.blendEnable = blendEnable;
				pipelineColorBlendAttachmentState.srcColorBlendFactor = srcColorBlendFactor;
				pipelineColorBlendAttachmentState.dstColorBlendFactor = dstColorBlendFactor;
				pipelineColorBlendAttachmentState.colorBlendOp = colorBlendOp;
				pipelineColorBlendAttachmentState.srcAlphaBlendFactor = srcAlphaBlendFactor;
				pipelineColorBlendAttachmentState.dstAlphaBlendFactor = dstAlphaBlendFactor;
				pipelineColorBlendAttachmentState.alphaBlendOp = alphaBlendOp;
				pipelineColorBlendAttachmentState.colorWriteMask = colorWriteMask;
				return pipelineColorBlendAttachmentState;
			}
			
			static inline VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo(
				//VkPipelineColorBlendStateCreateFlags          flags,
				VkBool32                                      logicOpEnable,
				VkLogicOp                                     logicOp,
				uint32_t                                      attachmentCount,
				const VkPipelineColorBlendAttachmentState*    pAttachments,
				float                                         blendConstants[4]
			) {
				VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
				pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
				//pipelineColorBlendStateCreateInfo.flags = flags;
				pipelineColorBlendStateCreateInfo.pNext = nullptr;
				pipelineColorBlendStateCreateInfo.logicOpEnable = logicOpEnable;
				pipelineColorBlendStateCreateInfo.logicOp = logicOp;
				pipelineColorBlendStateCreateInfo.attachmentCount = attachmentCount;
				pipelineColorBlendStateCreateInfo.pAttachments = pAttachments;
				pipelineColorBlendStateCreateInfo.blendConstants[0] = blendConstants[0];
				pipelineColorBlendStateCreateInfo.blendConstants[1] = blendConstants[1];
				pipelineColorBlendStateCreateInfo.blendConstants[2] = blendConstants[2];
				pipelineColorBlendStateCreateInfo.blendConstants[3] = blendConstants[3];
				return pipelineColorBlendStateCreateInfo;
			}
			
			static inline VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo(
				VkBool32                                  depthTestEnable,
				VkBool32                                  depthWriteEnable,
				VkCompareOp                               depthCompareOp,
				VkBool32                                  depthBoundsTestEnable = VK_FALSE,
				VkBool32                                  stencilTestEnable = VK_FALSE,
				VkStencilOpState                          front = {},
				VkStencilOpState                          back = {},
				float                                     minDepthBounds = 0.0f,
				float                                     maxDepthBounds = 0.0f
			)
			{
				VkPipelineDepthStencilStateCreateInfo depthStencil = {};
				depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
				depthStencil.depthTestEnable = depthTestEnable;
				depthStencil.depthWriteEnable = depthWriteEnable;
				depthStencil.depthCompareOp = depthCompareOp;// VK_COMPARE_OP_LESS
				depthStencil.depthBoundsTestEnable = depthBoundsTestEnable;
				depthStencil.stencilTestEnable = stencilTestEnable;
				depthStencil.front = front;
				depthStencil.back = back;
				depthStencil.minDepthBounds = minDepthBounds;
				depthStencil.maxDepthBounds = maxDepthBounds;
				return depthStencil;
			}

			static inline VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo(
				//VkPipelineInputAssemblyStateCreateFlags    flags,
				VkPrimitiveTopology                        topology,
				VkBool32                                   primitiveRestartEnable
			)
			{
				VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
				pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
				//pipelineInputAssemblyStateCreateInfo.flags = flags;
				pipelineInputAssemblyStateCreateInfo.topology = topology;
				pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = primitiveRestartEnable;
				return pipelineInputAssemblyStateCreateInfo;
			}
			static inline VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo(
				VkSampleCountFlagBits                    rasterizationSamples,
				VkBool32                                 sampleShadingEnable,
				float                                    minSampleShading = 0.0f,
				const VkSampleMask*                      pSampleMask = nullptr,
				VkBool32                                 alphaToCoverageEnable = VK_FALSE,
				VkBool32                                 alphaToOneEnable = VK_FALSE
			)
			{
				VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
				pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
				//pipelineInputAssemblyStateCreateInfo.flags = flags;
				pipelineMultisampleStateCreateInfo.pNext = nullptr;
				pipelineMultisampleStateCreateInfo.rasterizationSamples = rasterizationSamples;
				pipelineMultisampleStateCreateInfo.sampleShadingEnable = sampleShadingEnable;
				pipelineMultisampleStateCreateInfo.minSampleShading = minSampleShading;
				pipelineMultisampleStateCreateInfo.pSampleMask = pSampleMask;
				pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = alphaToCoverageEnable;
				pipelineMultisampleStateCreateInfo.alphaToOneEnable = alphaToOneEnable;
				return pipelineMultisampleStateCreateInfo;
			}

			
			static inline VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(
				//VkPipelineRasterizationStateCreateFlags    flags,
				VkBool32                                   depthClampEnable,
				VkBool32                                   rasterizerDiscardEnable,
				VkPolygonMode                              polygonMode,
				VkCullModeFlags                            cullMode,
				VkFrontFace                                frontFace,
				VkBool32                                   depthBiasEnable,
				float                                      depthBiasConstantFactor,
				float                                      depthBiasClamp,
				float                                      depthBiasSlopeFactor,
				float                                      lineWidth
			)
			{
				VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {};
				pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
				pipelineRasterizationStateCreateInfo.pNext = nullptr;
				//pipelineRasterizationStateCreateInfo.flags = flags;
				pipelineRasterizationStateCreateInfo.depthClampEnable = depthClampEnable;
				pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = rasterizerDiscardEnable;
				pipelineRasterizationStateCreateInfo.polygonMode = polygonMode;
				pipelineRasterizationStateCreateInfo.cullMode = cullMode;
				pipelineRasterizationStateCreateInfo.frontFace = frontFace;
				pipelineRasterizationStateCreateInfo.depthBiasEnable = depthBiasEnable;
				pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = depthBiasConstantFactor;
				pipelineRasterizationStateCreateInfo.depthBiasClamp = depthBiasClamp;
				pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = depthBiasSlopeFactor;
				pipelineRasterizationStateCreateInfo.lineWidth = lineWidth;
				return pipelineRasterizationStateCreateInfo;
			}

			static inline VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(VkShaderModule shader, VkShaderStageFlagBits stage)
			{
				VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
				vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				vertShaderStageInfo.stage = stage;
				vertShaderStageInfo.module = shader;
				vertShaderStageInfo.pName = "main";
				return vertShaderStageInfo;
			}

			static inline VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo(uint32_t patchControlPoints)
			{
				VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo{};
				pipelineTessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
				pipelineTessellationStateCreateInfo.patchControlPoints = patchControlPoints;
				return pipelineTessellationStateCreateInfo;
			}

			
			static inline VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo(
				//VkPipelineVertexInputStateCreateFlags       flags,
				uint32_t                                    vertexBindingDescriptionCount,
				const VkVertexInputBindingDescription*      pVertexBindingDescriptions,
				uint32_t                                    vertexAttributeDescriptionCount,
				const VkVertexInputAttributeDescription*    pVertexAttributeDescriptions
			) {
				VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
				pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
				pipelineVertexInputStateCreateInfo.pNext = nullptr;
				//pipelineVertexInputStateCreateInfo.flags = flags;
				pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = vertexBindingDescriptionCount;
				pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = pVertexBindingDescriptions;
				pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptionCount;
				pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = pVertexAttributeDescriptions;
				return pipelineVertexInputStateCreateInfo;
			}

			static inline VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo(
				//VkPipelineViewportStateCreateFlags    flags,
				uint32_t                              viewportCount,
				const VkViewport*                     pViewports,
				uint32_t                              scissorCount,
				const VkRect2D*                       pScissors
			) {
				VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
				pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
				pipelineViewportStateCreateInfo.pNext = nullptr;
				//pipelineViewportStateCreateInfo.flags = flags;
				pipelineViewportStateCreateInfo.viewportCount = viewportCount;
				pipelineViewportStateCreateInfo.pViewports = pViewports;
				pipelineViewportStateCreateInfo.scissorCount = scissorCount;
				pipelineViewportStateCreateInfo.pScissors = pScissors;
				return pipelineViewportStateCreateInfo;
			}
			

			static inline VkPushConstantRange pushConstantRange(VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size)
			{
				VkPushConstantRange pushConstantRange = {};
				pushConstantRange.offset = offset;
				pushConstantRange.size = size;
				pushConstantRange.stageFlags = stageFlags;
				return pushConstantRange;
			}

			static VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(
				const VkDynamicState* pDynamicStates,
				uint32_t dynamicStateCount,
				VkPipelineDynamicStateCreateFlags flags = 0)
			{
				VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
				pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
				pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates;
				pipelineDynamicStateCreateInfo.dynamicStateCount = dynamicStateCount;
				pipelineDynamicStateCreateInfo.flags = flags;
				return pipelineDynamicStateCreateInfo;
			}


			static inline VkSpecializationInfo specializationInfo(uint32_t mapEntryCount, const VkSpecializationMapEntry* mapEntries, size_t dataSize, const void* data)
			{
				VkSpecializationInfo specializationInfo{};
				specializationInfo.mapEntryCount = mapEntryCount;
				specializationInfo.pMapEntries = mapEntries;
				specializationInfo.dataSize = dataSize;
				specializationInfo.pData = data;
				return specializationInfo;
			}

			static inline VkSpecializationMapEntry specializationMapEntry(uint32_t constantID, uint32_t offset, uint32_t size)
			{
				VkSpecializationMapEntry specializationMapEntry = {};
				specializationMapEntry.constantID = constantID;
				specializationMapEntry.offset = offset;
				specializationMapEntry.size = size;
				return specializationMapEntry;
			}


			static inline VkSubpassDescription subpassDescription(
				//VkSubpassDescriptionFlags       flags,
				VkPipelineBindPoint             pipelineBindPoint,
				uint32_t                        inputAttachmentCount,
				const VkAttachmentReference*    pInputAttachments,
				uint32_t                        colorAttachmentCount,
				const VkAttachmentReference*    pColorAttachments,
				const VkAttachmentReference*    pResolveAttachments,
				const VkAttachmentReference*    pDepthStencilAttachment,
				uint32_t                        preserveAttachmentCount,
				const uint32_t*                 pPreserveAttachments
			)
			{
				VkSubpassDescription subpassDescription = {};
				subpassDescription.pipelineBindPoint = pipelineBindPoint;
				subpassDescription.inputAttachmentCount = inputAttachmentCount;
				subpassDescription.pInputAttachments = pInputAttachments;
				subpassDescription.colorAttachmentCount = colorAttachmentCount;
				subpassDescription.pColorAttachments = pColorAttachments;
				subpassDescription.pResolveAttachments = pResolveAttachments;
				subpassDescription.pDepthStencilAttachment = pDepthStencilAttachment;
				subpassDescription.preserveAttachmentCount = preserveAttachmentCount;
				return subpassDescription;
			}
			

			static inline VkSubpassDependency subpassDependency(
				uint32_t                srcSubpass,
				uint32_t                dstSubpass,
				VkPipelineStageFlags    srcStageMask,
				VkPipelineStageFlags    dstStageMask,
				VkAccessFlags           srcAccessMask,
				VkAccessFlags           dstAccessMask,
				VkDependencyFlags       dependencyFlags
			)
			{
				VkSubpassDependency subpassDependency = {};
				subpassDependency.srcSubpass = srcSubpass;
				subpassDependency.dstSubpass = dstSubpass;
				subpassDependency.srcStageMask = srcStageMask;
				subpassDependency.dstStageMask = dstStageMask;
				subpassDependency.srcAccessMask = srcAccessMask;
				subpassDependency.dstAccessMask = dstAccessMask;
				subpassDependency.dependencyFlags = dependencyFlags;
				return subpassDependency;
			}
			

			static inline VkRect2D rect2D(
				uint32_t offsetx,
				uint32_t offsety,
				uint32_t extendw,
				uint32_t extendh
			) {
				VkRect2D rect2D = {};
				rect2D.offset.x = offsetx;
				rect2D.offset.y = offsety;
				rect2D.extent.width = extendw;
				rect2D.extent.height = extendh;
				return rect2D;
			}

			static inline VkVertexInputAttributeDescription vertexInputAttributeDescription(
				uint32_t             location,
				uint32_t             binding,
				VkFormat             format,
				uint32_t             offset
			) {
				VkVertexInputAttributeDescription vertexInputAttributeDescription = {};
				vertexInputAttributeDescription.location = location;
				vertexInputAttributeDescription.binding = binding;
				vertexInputAttributeDescription.format = format; 
				vertexInputAttributeDescription.offset = offset;
				return vertexInputAttributeDescription;
			}
			

			
			static inline VkVertexInputBindingDescription vertexInputBindingDescription(
				uint32_t             binding,
				uint32_t             stride,
				VkVertexInputRate    inputRate
			) {
				VkVertexInputBindingDescription vertexInputBindingDescription = {};
				vertexInputBindingDescription.binding = binding;
				vertexInputBindingDescription.stride = stride;
				vertexInputBindingDescription.inputRate = inputRate;
				return vertexInputBindingDescription;
			}
			
			static inline VkViewport viewport(
				float x,
				float y,
				float width,
				float height,
				float minDepth = 0.0f,
				float maxDepth = 1.0f
			) {
				VkViewport viewport = {};
				viewport.x = x;
				viewport.y = y;
				viewport.width = width;
				viewport.height = height;
				viewport.minDepth = minDepth;
				viewport.maxDepth = maxDepth;
				return viewport;
			}


			


			static inline VkWriteDescriptorSet writeDescriptorSet(
				VkDescriptorSet                  dstSet,
				uint32_t                         dstBinding,
				uint32_t                         dstArrayElement,
				uint32_t                         descriptorCount,
				VkDescriptorType                 descriptorType,
				const VkDescriptorImageInfo*     pImageInfo,
				const VkDescriptorBufferInfo*    pBufferInfo,
				const VkBufferView*              pTexelBufferView
			)
			{
				VkWriteDescriptorSet descriptorWrite;
				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.pNext = nullptr;
				descriptorWrite.dstSet = dstSet;
				descriptorWrite.dstBinding = dstBinding;
				descriptorWrite.dstArrayElement = dstArrayElement;
				descriptorWrite.descriptorCount = descriptorCount;
				descriptorWrite.descriptorType = descriptorType;
				descriptorWrite.pImageInfo = pImageInfo;
				descriptorWrite.pBufferInfo = pBufferInfo;
				descriptorWrite.pTexelBufferView = pTexelBufferView;
				return descriptorWrite;
			}


			static inline VkWriteDescriptorSet writeDescriptorSet(
				VkDescriptorSet                  dstSet, 
				uint32_t                         dstBinding,
				VkDescriptorType                 descriptorType,
				const VkDescriptorBufferInfo*    pBufferInfo
			)
			{
				return writeDescriptorSet(dstSet, dstBinding, 0, 1, descriptorType, nullptr, pBufferInfo, nullptr);
			}

			/*
			
			 descriptorType: VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER/
			*/ 
			static inline VkWriteDescriptorSet writeDescriptorSet(
				VkDescriptorSet                  dstSet,
				uint32_t                         dstBinding,
				VkDescriptorType                 descriptorType,
				const VkDescriptorImageInfo*     pImageInfo
			)
			{
				return writeDescriptorSet(dstSet, dstBinding, 0, 1, descriptorType, pImageInfo, nullptr, nullptr);
			}







		}

	}


}
#endif // !CRAFT_ENGINE_VYLKAN_API_H_