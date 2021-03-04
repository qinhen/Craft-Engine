#pragma once
#ifndef CRAFT_ENGINE_VULKAN_IMAGE_COMPUTER_H_
#define CRAFT_ENGINE_VULKAN_IMAGE_COMPUTER_H_

#include "./VulkanApi.h"
#include "../Util/Core.h"

namespace CraftEngine
{
	namespace Vulkan
	{

		class ComputePipeline
		{
			uint32_t m_storageImageCount;
			uint32_t m_uniformBufferCount;
			uint32_t m_storageBufferCount;

			VkShaderModule m_shader;

			VkDescriptorSetLayout m_descriptorSetLayout;
			VkPipelineLayout m_pipelineLayout;
			VkPipeline m_pipeline;

			VkDescriptorSet m_descriptorSet;
			DescriptorPool m_descriptorPool;
			VkCommandBuffer m_commmandBuffer = VK_NULL_HANDLE;
			LogicalDevice* m_vkDevice;

			std::vector<ImageReference> m_storageImageRef;
			std::vector<BufferReference> m_uniformBufferRef;
			std::vector<BufferReference> m_storageBufferRef;
	
		public:

			const VkCommandBuffer& command()const { return m_commmandBuffer; }


			void clear()
			{
				m_vkDevice->destroy(m_descriptorPool);
				m_vkDevice->destroyCommandBuffers(&m_commmandBuffer, 1);
			}

			void init(
				LogicalDevice* vkDevice, 
				uint32_t storageImageCount,
				uint32_t uniformBufferCount,
				uint32_t storageBufferCount,
				VkShaderModule shader
			)
			{
				m_vkDevice = vkDevice;

				m_storageImageCount = storageImageCount;
				m_uniformBufferCount = uniformBufferCount;
				m_storageBufferCount = storageBufferCount;
				m_shader = shader;

				m_storageImageRef.resize(storageImageCount);
				m_uniformBufferRef.resize(uniformBufferCount);
				m_storageBufferRef.resize(storageBufferCount);

				// build pipeline
				int binding_index = 0;
				std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings;
				for (int i = 0; i < m_storageImageCount; i++, binding_index++)
					setLayoutBindings.push_back(VulkanConstructor::descriptorSetLayoutBinding(binding_index, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT));
				for (int i = 0; i < m_uniformBufferCount; i++, binding_index++)
					setLayoutBindings.push_back(VulkanConstructor::descriptorSetLayoutBinding(binding_index, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT));
				for (int i = 0; i < m_storageBufferCount; i++, binding_index++)
					setLayoutBindings.push_back(VulkanConstructor::descriptorSetLayoutBinding(binding_index, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT));

				this->m_descriptorSetLayout = m_vkDevice->createDescriptorSetLayout(setLayoutBindings.data(), setLayoutBindings.size());
				VkDescriptorSetLayout descriptorSetLayouts[] = { this->m_descriptorSetLayout };
				m_pipelineLayout = m_vkDevice->createPipelineLayout(descriptorSetLayouts, 1, 0, nullptr);

				VkPipelineShaderStageCreateInfo compShaderStageInfo = {};
				compShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				compShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
				compShaderStageInfo.module = m_shader;
				compShaderStageInfo.pName = "main";

				VkComputePipelineCreateInfo computePipelineCreateInfo = {};

				computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
				computePipelineCreateInfo.layout = m_pipelineLayout;
				computePipelineCreateInfo.flags = 0;

				computePipelineCreateInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				computePipelineCreateInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
				computePipelineCreateInfo.stage.module = m_shader;
				computePipelineCreateInfo.stage.pName = "main";
				computePipelineCreateInfo.stage.pSpecializationInfo = nullptr;
				computePipelineCreateInfo.stage.flags = 0;
				computePipelineCreateInfo.stage.pNext = nullptr;

				if (vkCreateComputePipelines(m_vkDevice->logicalDevice(), VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &m_pipeline) != VK_SUCCESS)
					throw std::runtime_error("failed to create compute pipeline!");


				// prepare rescources
				m_vkDevice->createCommandBuffers(1, &m_commmandBuffer);
				VkDescriptorPoolSize pool_size[3] = {};
				std::vector<VkDescriptorPoolSize> pool_sizes;
				if (m_storageImageCount > 0)
					pool_sizes.push_back({ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE , m_storageImageCount });
				if (m_uniformBufferCount > 0)
					pool_sizes.push_back({ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER , m_uniformBufferCount });
				if (m_storageBufferCount > 0)
					pool_sizes.push_back({ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER , m_storageBufferCount });

				m_descriptorPool = m_vkDevice->createDescriptorPool(pool_sizes.data(), pool_sizes.size(), 1);
				m_descriptorPool.allocDescriptorSets(&m_descriptorSetLayout, &m_descriptorSet, 1);

			}


			void updateStorageImage(uint32_t binding, ImageReference image)
			{
				if (binding > m_storageImageCount)
					throw std::runtime_error("binding out of range!");

				m_storageImageRef[binding] = image;

				auto info = VulkanConstructor::descriptorImageInfo(VK_NULL_HANDLE, image.view, VK_IMAGE_LAYOUT_GENERAL);
				auto write = VulkanConstructor::writeDescriptorSet(m_descriptorSet, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, binding, &info);
				m_vkDevice->updateDescriptorSets(1, &write, 0, nullptr);
			}

			void updateUniformBuffer(uint32_t binding, BufferReference buffer)
			{
				//if (binding > m_storageImageCount)
				//	throw std::runtime_error("binding out of range!");
				m_uniformBufferRef[binding + m_storageImageCount] = buffer;
				auto info = VulkanConstructor::descriptorBufferInfo(buffer.buffer, 0, buffer.size);
				auto write = VulkanConstructor::writeDescriptorSet(m_descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, binding, &info);
				m_vkDevice->updateDescriptorSets(1, &write, 0, nullptr);
			}

			void updateStorageBuffer(uint32_t binding, BufferReference buffer)
			{
				//if (binding > m_storageImageCount)
				//	throw std::runtime_error("binding out of range!");
				m_uniformBufferRef[binding + m_storageImageCount + m_uniformBufferCount] = buffer;
				auto info = VulkanConstructor::descriptorBufferInfo(buffer.buffer, 0, buffer.size);
				auto write = VulkanConstructor::writeDescriptorSet(m_descriptorSet, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, binding, &info);
				m_vkDevice->updateDescriptorSets(1, &write, 0, nullptr);
			}




			void createCommandBuffer(uint32_t groupx, uint32_t groupy = 1, uint32_t groupz = 1)
			{


				VkCommandBufferBeginInfo beginInfo = {};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

				vkBeginCommandBuffer(m_commmandBuffer, &beginInfo);

				for (auto i : m_storageImageRef)
				{
					VkImageMemoryBarrier imageBarrier = {};
					imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
					imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					imageBarrier.image = i.image;
					imageBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
					imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
					imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					imageBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
					imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					imageBarrier.subresourceRange.baseArrayLayer = 0;
					imageBarrier.subresourceRange.baseMipLevel = 0;
					imageBarrier.subresourceRange.layerCount = 1;
					imageBarrier.subresourceRange.levelCount = 1;
					vkCmdPipelineBarrier(
						m_commmandBuffer,
						VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
						VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						0,
						0, nullptr,
						0, nullptr,
						1, &imageBarrier);
				}
	
				for (auto b : m_storageBufferRef)
				{
					VkBufferMemoryBarrier bufferBarrier{};
					bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
					bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					bufferBarrier.buffer = b.buffer;
					bufferBarrier.size = b.size;
					bufferBarrier.srcAccessMask = VK_ACCESS_UNIFORM_READ_BIT;
					bufferBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
					bufferBarrier.srcQueueFamilyIndex = 0;//
					bufferBarrier.dstQueueFamilyIndex = 0;//

					vkCmdPipelineBarrier(
						m_commmandBuffer,
						VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
						VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						0,
						0, nullptr,
						1, &bufferBarrier,
						0, nullptr);
				}

				vkCmdBindPipeline(m_commmandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline);
				vkCmdBindDescriptorSets(m_commmandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipelineLayout, 0, 1, &m_descriptorSet, 0, 0);
				vkCmdDispatch(m_commmandBuffer, groupx, groupy, groupz);

				for (auto i : m_storageImageRef)
				{
					VkImageMemoryBarrier imageBarrier = {};
					imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
					imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					imageBarrier.image = i.image;
					imageBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
					imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
					imageBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
					imageBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					imageBarrier.subresourceRange.baseArrayLayer = 0;
					imageBarrier.subresourceRange.baseMipLevel = 0;
					imageBarrier.subresourceRange.layerCount = 1;
					imageBarrier.subresourceRange.levelCount = 1;
					vkCmdPipelineBarrier(
						m_commmandBuffer,
						VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
						VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						0,
						0, nullptr,
						0, nullptr,
						1, &imageBarrier);
				}

				for (auto b : m_storageBufferRef)
				{
					VkBufferMemoryBarrier bufferBarrier{};
					bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
					bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					bufferBarrier.buffer = b.buffer;
					bufferBarrier.size = b.size;
					bufferBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
					bufferBarrier.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT;
					bufferBarrier.srcQueueFamilyIndex = 0;//
					bufferBarrier.dstQueueFamilyIndex = 0;//

					vkCmdPipelineBarrier(
						m_commmandBuffer,
						VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
						VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						0,
						0, nullptr,
						1, &bufferBarrier,
						0, nullptr);
				}

				if (vkEndCommandBuffer(m_commmandBuffer) != VK_SUCCESS) {
					throw std::runtime_error("failed to record command buffer!");
				}

			}


		};



		class FilterComputePipeline
		{
		private:
			ComputePipeline m_pipeline;
			uint32_t m_width;
			uint32_t m_height;
			LogicalDevice* m_vkDevice;
			VkShaderModule m_shader;

			ImageTexture m_input = {};
			ImageTexture m_output;
			bool m_outputValid = false;
		public:
			//void clear()
			//{
			//	m_pipeline.clear();
			//	if (m_outputValid)
			//	{
			//		m_vkDevice->destroy(m_output);
			//	}
			//}

			void init(LogicalDevice* device, VkShaderModule shader)
			{
				m_vkDevice = device;
				m_shader = shader;
				m_pipeline.init(device, 2, 0, 0, m_shader);
			}

			void setSize(uint32_t width, uint32_t height)
			{
				if (m_width != width || m_height != height)
				{
					m_width = width;
					m_height = height;
					if (m_outputValid == true)
					{
						m_vkDevice->destroy(m_output);
					}
					m_output = m_vkDevice->createColorAttachmentImage(width, height, ImageFormat::format_rgba8_unorm);
					m_pipeline.updateStorageImage(1, m_output.reference());
					m_outputValid = true;
				}
			}

			void submit(ImageTexture input)
			{
				if (m_width != input.mWidth || m_height != input.mHeight)
				{
					throw std::runtime_error("unsatisfied output image!");
				}
				m_input = input;
				m_pipeline.updateStorageImage(0, m_input.reference());	
				uint32_t groupSize[2] = { 1, 1 };
				groupSize[0] = m_width / 16 + (m_width % 16 > 0 ? 1 : 0);
				groupSize[1] = m_height / 16 + (m_height % 16 > 0 ? 1 : 0);
				m_pipeline.createCommandBuffer(groupSize[0], groupSize[1], 1);
				VkCommandBuffer cmdbuf = m_pipeline.command();
				m_vkDevice->executeCommands(0, &cmdbuf, 1);	
			}

			const ImageTexture* getOutput()
			{
				if (m_outputValid)
					return &m_output;
				else
					return nullptr;
			}

		};



		class EdgeDetectPipeline: public FilterComputePipeline
		{
		private:

		public:
			void clear()
			{
				// auto shaderCode = CraftEngine::Util::readFile("shaders/edgedetect_comp.spv");
			}
			void init(LogicalDevice* device)
			{
				auto shaderCode = CraftEngine::Util::readFile("shaders/edgedetect_comp.spv");
				auto shader = device->createShaderModule(shaderCode.data(), shaderCode.size());
				FilterComputePipeline::init(device, shader);
			}
		};

		class EmbossPipeline : public FilterComputePipeline
		{
		private:

		public:
			void clear()
			{
				// auto shaderCode = CraftEngine::Util::readFile("shaders/edgedetect_comp.spv");
			}
			void init(LogicalDevice* device)
			{
				auto shaderCode = CraftEngine::Util::readFile("shaders/emboss_comp.spv");
				auto shader = device->createShaderModule(shaderCode.data(), shaderCode.size());
				FilterComputePipeline::init(device, shader);
			}
		};

		class SharpenPipeline : public FilterComputePipeline
		{
		private:

		public:
			void clear()
			{
				// auto shaderCode = CraftEngine::Util::readFile("shaders/edgedetect_comp.spv");
			}
			void init(LogicalDevice* device)
			{
				auto shaderCode = CraftEngine::Util::readFile("shaders/sharpen_comp.spv");
				auto shader = device->createShaderModule(shaderCode.data(), shaderCode.size());
				FilterComputePipeline::init(device, shader);
			}
		};

	}
}

#endif