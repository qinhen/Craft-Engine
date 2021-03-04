#pragma once
#ifndef CRAFT_ENGINE_GUI_MEDIA_PLAYER_WIDGET_H_
#define CRAFT_ENGINE_GUI_MEDIA_PLAYER_WIDGET_H_
/*
* 
* 
* 
*/
#include "../widgets/Widget.h"
#include "../../advance/vulkan/VulkanSystem.h"

namespace CraftEngine
{
	namespace gui
	{


		class VulkanWidget :public Widget
		{
		private:
			struct Image {
				vulkan::ImageTexture image;
				VkDescriptorSet set;
			};
			vulkan::LogicalDevice* m_logicalDevice;
			Image m_image;
			vulkan::DescriptorPool m_descriptorPool;
			vulkan::CommandPool m_cmdPool;
			VkSampler m_sampler;
			HandleImage m_imageHandle;
			VkDescriptorSetLayout image_descriptor_set_layout;
		public:
			craft_engine_gui_signal(drawFrame, void(void));

			VulkanWidget(Widget* parent): Widget(parent)
			{
				m_logicalDevice = vulkan::VulkanSystem::getLogicalDevice();
				VkDescriptorSetLayoutBinding setLayoutBinding =
					vulkan::vkt::descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
				image_descriptor_set_layout = m_logicalDevice->createDescriptorSetLayout(&setLayoutBinding, 1);
				auto pool_size = vulkan::vkt::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
				m_descriptorPool = m_logicalDevice->createDescriptorPool(&pool_size, 1, 1);
				m_descriptorPool.create(&image_descriptor_set_layout, &m_image.set, 1);
				m_cmdPool = m_logicalDevice->createCommandPool();
				m_sampler = m_logicalDevice->createSampler();
			}

			~VulkanWidget()
			{
				m_logicalDevice->destroyDescriptorSetLayout(image_descriptor_set_layout);
				m_logicalDevice->destroy(m_descriptorPool);
				m_logicalDevice->destroySampler(m_sampler);
			}

			void clearBinding()
			{
				m_imageHandle = HandleImage(nullptr);
			}

			void bindImage(VkImageView image_view)
			{
				clearBinding();
				if (image_view != VK_NULL_HANDLE)
				{
					m_image.image.mView = image_view;
					VkDescriptorImageInfo image_info = vulkan::vkt::descriptorImageInfo(
						m_sampler, m_image.image.mView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
					VkWriteDescriptorSet buffer_write = {};
					buffer_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					buffer_write.dstSet = m_image.set;
					buffer_write.dstBinding = 0;
					buffer_write.dstArrayElement = 0;
					buffer_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					buffer_write.descriptorCount = 1;
					buffer_write.pImageInfo = &image_info;
					m_logicalDevice->updateDescriptorSets(1, &buffer_write, 0, nullptr);
					m_imageHandle = HandleImage(&m_image);
				}
			}

			void onPaintEvent(const PaintEvent& paintEvent)
			{
				craft_engine_gui_emit(drawFrame);
				GuiRenderer* renderer = paintEvent.pRenderer;
				const Point& global_relative_point = paintEvent.globalRelativePoint;
				const Rect& global_limit_rect = paintEvent.globalLimitRect;
				Widget::onPaintEvent(paintEvent);
				auto this_global_rect = Rect(global_relative_point + getPos(), getSize());
				renderer->drawRect(this_global_rect, m_imageHandle, global_limit_rect);
			}

		};


	}
}

#endif // CRAFT_ENGINE_GUI_MEDIA_PLAYER_WIDGET_H_