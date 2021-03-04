#pragma once
#include "./vk_gui_renderer.h"

namespace CraftEngine
{
	namespace gui
	{

		namespace _Vulkan_Render_System_Detail
		{



			class VulkanGuiCanvasRenderer : public AbstractVulkanGuiRenderer
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
					clearColor[0].color.float32[3] = 0.0;
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

					vulkan::makeCmdTransitionImageLayout(
						m_cmdBuffer, m_swapchainImages[m_curFramebufferIndex],
						VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

					if (vkEndCommandBuffer(m_cmdBuffer) != VK_SUCCESS) {
						throw std::runtime_error("failed to record command buffer!");
					}
					m_pLogicalDevice->executeCommands(m_pLogicalDevice->getGraphicsQueueIndex(), &m_cmdBuffer, 1);
				}
			};

			

			class VulkanGuiCanvas : public AbstractGuiCanvas
			{
			private:
				_Vulkan_Render_System_Detail::VulkanGuiCanvasRenderer* m_renderer;
				struct Image
				{
					vulkan::ImageTexture m_image;
					VkDescriptorSet      m_set;
				};
				VkSampler m_sampler; 
				Image m_colorAttachment;
				uint32_t m_width;
				uint32_t m_height;
				vulkan::DescriptorPool m_descriptorPool;
				vulkan::LogicalDevice* m_pLogicalDevice;

			public:

				void init(uint32_t width, uint32_t height, vulkan::LogicalDevice* pLogicalDevice)
				{
					m_width = width;
					m_height = height;
					m_pLogicalDevice = pLogicalDevice;
					m_colorAttachment.m_image = m_pLogicalDevice->createColorAttachmentImage(width, height, vulkan::ImageFormat::format_rgba8_unorm);
					m_sampler = m_pLogicalDevice->createSampler();

					m_renderer = new VulkanGuiCanvasRenderer;
					m_renderer->init(m_width, m_height, { m_colorAttachment.m_image.mImage }, { m_colorAttachment.m_image.mView }, VK_FORMAT_R8G8B8A8_UNORM, m_pLogicalDevice, 4);
					m_renderer->setFrameBufferIndex(0);

					VkDescriptorPoolSize poolSize;
					poolSize.descriptorCount = 1;
					poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					m_descriptorPool = m_pLogicalDevice->createDescriptorPool(&poolSize, 1, 1);
					auto descriptorSetLayout = m_renderer->m_pipeline.getImageUniformBufferDescriptorSetLayout();
					m_descriptorPool.create(&descriptorSetLayout, &m_colorAttachment.m_set, 1);
					VkDescriptorImageInfo imageInfo = vulkan::vkt::descriptorImageInfo(m_sampler, m_colorAttachment.m_image.mView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
					VkWriteDescriptorSet bufferWrite = vulkan::vkt::writeDescriptorSet(m_colorAttachment.m_set, 0, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &imageInfo, nullptr, nullptr);
					m_pLogicalDevice->updateDescriptorSets(1, &bufferWrite, 0, nullptr);
				}
				void clear()
				{
					m_renderer->clear();
					delete m_renderer;
					m_descriptorPool.destroy(&m_colorAttachment.m_set, 1);
					m_pLogicalDevice->destroy(m_descriptorPool);
					m_pLogicalDevice->destroySampler(m_sampler);
					m_pLogicalDevice->destroy(m_colorAttachment.m_image);
				}

				virtual Size getCanvasSize() override
				{
					return Size(m_width, m_height);
				}
				virtual void setCanvasSize(const Size& size) override
				{
					m_width = size.x;
					m_height = size.y;
					m_pLogicalDevice->destroy(m_colorAttachment.m_image);
					m_colorAttachment.m_image = m_pLogicalDevice->createColorAttachmentImage(m_width, m_height, vulkan::ImageFormat::format_rgba8_unorm);
					m_renderer->resetRenderTarget(m_width, m_height, { m_colorAttachment.m_image.mImage }, { m_colorAttachment.m_image.mView });
				}
				virtual void setCurrentFramebufferIndex(int32_t idx) override
				{
					m_renderer->setFrameBufferIndex(0);
				}
				virtual uint32_t getCurrentFramebufferIndex() const override
				{
					return 0;
				}
				virtual HandleImage getFramebufferImage(uint32_t attachmentIdx = 0, uint32_t framebufferIdx = 0)  override
				{
					return HandleImage(&m_colorAttachment);
				}
				virtual uint32_t getFramebufferCount() const  override
				{
					return 1;
				}
				virtual uint32_t getAttachmentCount() const override
				{
					return 1;
				}
				virtual AbstractGuiRenderer* getGuiRenderer() override
				{
					return m_renderer;
				}
			};

		}

	};



}
