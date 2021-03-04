#pragma once
#include "../Common.h"
#include "./vk_gui_pipeline.h"

namespace CraftEngine
{
	namespace gui
	{

		namespace _Vulkan_Render_System_Detail
		{

			class AbstractVulkanGuiRenderer : public CraftEngine::gui::AbstractGuiRenderer
			{
			public:
				struct Image {
					vulkan::ImageTexture image;
					VkDescriptorSet set;
				};

				vulkan::LogicalDevice* m_pLogicalDevice;
				VulkanGuiRendererPipeline m_pipeline;
				vulkan::BufferMemory m_vertexBuffer;
				vulkan::BufferMemory m_indexBuffer;
				uint32_t m_vertexCount;
				uint32_t m_indexCount;
				uint32_t m_renderWidth;
				uint32_t m_renderHeight;
				vulkan::CommandPool m_cmdPool;
				VkCommandBuffer m_cmdBuffer = VK_NULL_HANDLE;
							
				std::vector<VkFramebuffer> m_framebuffers;
				std::vector<VkImage>       m_swapchainImages;
				std::vector<VkImageView>   m_swapchainImageViews;
				bool                       m_isMultiSampleTargetCreated = false;
				vulkan::ImageTexture       m_multiSampleTarget;
				VkFormat m_swapchainImageFormat;
				int m_multiSampleCount;

				int32_t       m_curFramebufferIndex = -1;

			public:

				vulkan::LogicalDevice* getLogicalDevice() {
					return m_pLogicalDevice;
				}

				void init(
					uint32_t initWidth,
					uint32_t initHeight,
					const std::vector<VkImage>&     swapchainImages,
					const std::vector<VkImageView>& swapchainImageViews,
					VkFormat swapchainImageFormat,
					vulkan::LogicalDevice* pLogicalDevice,
					int multiSampleCount
				)
				{
					m_renderWidth = initWidth;
					m_renderHeight = initHeight;
					m_pLogicalDevice = pLogicalDevice;
					m_swapchainImageFormat = swapchainImageFormat;
					// MSAA
					m_multiSampleCount = multiSampleCount < m_pLogicalDevice->getParent()->getMaxMSAASampleCount() ? multiSampleCount : m_pLogicalDevice->getParent()->getMaxMSAASampleCount();
					m_pipeline.init(pLogicalDevice, m_multiSampleCount); // MSAA
					m_pipeline.create(swapchainImageFormat);

					m_cmdPool = m_pLogicalDevice->createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
					m_vertexCount = 1024;
					m_vertexBuffer = m_pLogicalDevice->createVertexBuffer(m_cmdPool, nullptr, sizeof(Vertex) * 1024);//
					m_indexCount = 1024;
					m_indexBuffer = m_pLogicalDevice->createIndexBuffer(m_cmdPool, nullptr, sizeof(uint32_t) * 1024);//

					resetRenderTarget(initWidth, initHeight, swapchainImages, swapchainImageViews);
				}


				virtual void clear()override
				{
					m_pLogicalDevice->destroy(m_indexBuffer);
					m_pLogicalDevice->destroy(m_vertexBuffer);
					m_pLogicalDevice->destroy(m_cmdPool);

					for (int i = 0; i < m_framebuffers.size(); i++)
						m_pLogicalDevice->destroyFramebuffer(m_framebuffers[i]);
					m_framebuffers.clear();

					if (m_isMultiSampleTargetCreated)
					{
						m_pLogicalDevice->destroy(m_multiSampleTarget);
						m_isMultiSampleTargetCreated = false;
					}
		
					m_pipeline.clear();
				}


				void resetRenderTarget(
					uint32_t initWidth,
					uint32_t initHeight,
					const std::vector<VkImage>& swapchainImages,
					const std::vector<VkImageView>& swapchainImageViews
				) {
					m_renderWidth = initWidth;
					m_renderHeight = initHeight;

					m_swapchainImages = swapchainImages;
					m_swapchainImageViews = swapchainImageViews;

					if (m_isMultiSampleTargetCreated)
					{
						m_pLogicalDevice->destroy(m_multiSampleTarget);
						m_isMultiSampleTargetCreated = false;
					}
					m_multiSampleTarget = m_pLogicalDevice->createColorAttachmentImage(m_renderWidth, m_renderHeight,
						(vulkan::ImageFormat)m_swapchainImageFormat, (VkSampleCountFlagBits)m_multiSampleCount);
					m_isMultiSampleTargetCreated = true;

					for (int i = 0; i < m_framebuffers.size(); i++)
						m_pLogicalDevice->destroyFramebuffer(m_framebuffers[i]);
					m_framebuffers.resize(swapchainImageViews.size());

					for (int i = 0; i < m_framebuffers.size(); i++)
					{
						core::ArrayList<VkImageView> attachments;
						if (m_multiSampleCount > 1)
							attachments.push_back(m_multiSampleTarget.mView);
						attachments.push_back(swapchainImageViews[i]);
						m_framebuffers[i] = m_pLogicalDevice->createFramebuffer(
							m_pipeline.getRenderPass(),
							attachments.size(),
							attachments.data(),
							m_renderWidth,
							m_renderHeight,
							1);
					}
				}


				void setFrameBufferIndex(uint32_t index)
				{
					m_curFramebufferIndex = index;
				}

				VkCommandBuffer getCmdBuf() { return m_cmdBuffer; }

			protected:
				void copyDrawData()
				{
					if (m_drawList.mVerticesData.size() > m_vertexCount)
					{
						m_pLogicalDevice->destroy(m_vertexBuffer);
						m_vertexCount = m_drawList.mVerticesData.size();
						m_vertexBuffer = m_pLogicalDevice->createVertexBuffer(m_cmdPool, nullptr, sizeof(Vertex) * m_vertexCount);//
					}
					if (m_drawList.mIndicesData.size() > m_indexCount)
					{
						m_pLogicalDevice->destroy(m_indexBuffer);
						m_indexCount = m_drawList.mIndicesData.size();
						m_indexBuffer = m_pLogicalDevice->createIndexBuffer(m_cmdPool, nullptr, sizeof(uint32_t) * m_indexCount);//
					}
					m_pLogicalDevice->copyDataToBuffer(m_cmdPool, m_vertexBuffer.mBuffer, m_drawList.mVerticesData.data(), sizeof(Vertex) * m_drawList.mVerticesData.size());
					m_pLogicalDevice->copyDataToBuffer(m_cmdPool, m_indexBuffer.mBuffer, m_drawList.mIndicesData.data(), sizeof(uint32_t) * m_drawList.mIndicesData.size());
				}

			};




			class VulkanGuiRenderer : public AbstractVulkanGuiRenderer
			{
			public:


				virtual void begin(void* userData)
				{
					m_drawList.mDrawCommand.clear();
					m_drawList.mVerticesData.clear();
					m_drawList.mIndicesData.clear();
				}


				virtual void end(void* userData)
				{
					if (m_cmdBuffer == VK_NULL_HANDLE)
						m_cmdPool.create(1, &m_cmdBuffer);
					else
					{
						m_cmdPool.destroy(&m_cmdBuffer, 1);
						m_cmdBuffer = VK_NULL_HANDLE;
						m_cmdPool.create(1, &m_cmdBuffer);
					}

					if (m_drawList.mDrawCommand.size() > 0)
						copyDrawData();

					VkCommandBufferBeginInfo beginInfo = {};
					beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
					beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

					Color background_color = GuiColorStyle::getBackgroundColor();

					VkClearValue clearColor[2];
					clearColor[0].color.float32[0] = background_color.r / 255.0f;
					clearColor[0].color.float32[1] = background_color.b / 255.0f;
					clearColor[0].color.float32[2] = background_color.g / 255.0f;
					clearColor[0].color.float32[3] = background_color.a / 255.0f;
					clearColor[1] = clearColor[0];

					vkBeginCommandBuffer(m_cmdBuffer, &beginInfo);
					VkRenderPassBeginInfo renderPassInfo = {};
					renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
					renderPassInfo.renderPass = m_pipeline.getRenderPass();
					renderPassInfo.framebuffer = m_framebuffers[m_curFramebufferIndex];
					renderPassInfo.renderArea.offset = { 0, 0 };
					renderPassInfo.renderArea.extent = { m_renderWidth, m_renderHeight };
					renderPassInfo.clearValueCount = m_multiSampleCount > 1 ? 2 : 1;
					renderPassInfo.pClearValues = clearColor;

					VkViewport viewport = { 0.0f, 0.0f, (float)m_renderWidth, (float)m_renderHeight, 0.0f, 1.0f };
					vkCmdSetViewport(m_cmdBuffer, 0, 1, &viewport);
					VkRect2D scissor = { 0, 0, m_renderWidth, m_renderHeight };
					vkCmdSetScissor(m_cmdBuffer, 0, 1, &scissor);
					vkCmdSetLineWidth(m_cmdBuffer, 1.0f);

					vkCmdBeginRenderPass(m_cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
					VkDeviceSize offsets[1] = { 0 };

					vkCmdBindVertexBuffers(m_cmdBuffer, 0, 1, &m_vertexBuffer.mBuffer, offsets);
					vkCmdBindIndexBuffer(m_cmdBuffer, m_indexBuffer.mBuffer, 0, VK_INDEX_TYPE_UINT32);
					
					const mat4 projectionMatrix = math::orthoWorldLH_DeviceRH<float>(0.0f, m_renderWidth, 0.0f, m_renderHeight, 0.0f, 2.0f);
					mat4 transformMatrix;
					auto pipeline = m_pipeline.getPipeline(0);

					HandleImage current_image = HandleImage(nullptr);
					int current_pipeline = 0;
					vkCmdBindPipeline(m_cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
					for (auto& draw_buffer : m_drawList.mDrawCommand)
					{
						if (current_pipeline != draw_buffer.mPipeline)
						{
							current_pipeline = draw_buffer.mPipeline;
							switch (draw_buffer.mPipeline)
							{
							case 0:
								pipeline = m_pipeline.getPipeline(0);
								break;
							case 1:
								pipeline = m_pipeline.getPipeline(1);
								break;
							case 2:
								pipeline = m_pipeline.getPipeline(0);
								break;
							}
							vkCmdBindPipeline(m_cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
						}

						transformMatrix = projectionMatrix * draw_buffer.mTransform;
						vkCmdPushConstants(m_cmdBuffer, m_pipeline.getPipelineLayout(0), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(transformMatrix), &transformMatrix);

						if (current_image != draw_buffer.mImage)
						{
							Image* pImage = (Image*)draw_buffer.mImage.unused;
							vkCmdBindDescriptorSets(m_cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.getPipelineLayout(0), 0, 1, &pImage->set, 0, nullptr);
							current_image = draw_buffer.mImage;
						}
						vkCmdSetScissor(m_cmdBuffer, 0, 1, (const VkRect2D*)&draw_buffer.mScissor);
						vkCmdDrawIndexed(m_cmdBuffer, draw_buffer.mIndexCount, 1, draw_buffer.mIndexOffset, draw_buffer.mVertexOffset, 0);
					}


					vkCmdEndRenderPass(m_cmdBuffer);
					if (vkEndCommandBuffer(m_cmdBuffer) != VK_SUCCESS) {
						throw std::runtime_error("failed to record command buffer!");
					}
				}


			};





		}

	};

}
