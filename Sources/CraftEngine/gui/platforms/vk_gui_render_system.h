#pragma once

#include <array>
#include "../Common.h"
#include "./vk_gui_canvas.h"


namespace CraftEngine
{
	namespace gui
	{

		namespace _Vulkan_Render_System_Detail
		{
			constexpr const uint32_t MAX_IMAGE_COUNT = 4096;

			class VulkanGuiRenderSystem :public AbstractGuiRenderSystem
			{
			private:

				VkDescriptorSetLayout m_imageDescriptorSetLayout;
				struct Image {
					vulkan::ImageTexture image;
					VkDescriptorSet set;
				};

				core::ObjectPool<Image, MAX_IMAGE_COUNT> m_imagePool;
				vulkan::DescriptorPool m_imageDescriptorPool;
				vulkan::LogicalDevice* m_pLogicalDevice;
				std::unordered_map<std::thread::id, vulkan::CommandPool> m_cmdPoolsMap;
				std::mutex m_cmdPoolMutex;
				VkSampler m_sampler;
				VkSampler m_sdf_sampler;
				const vulkan::CommandPool& getCommandPool()
				{
					std::lock_guard<std::mutex> lock(m_cmdPoolMutex);
					auto thread_id = std::this_thread::get_id();
					auto pool_it = m_cmdPoolsMap.find(thread_id);
					if (pool_it == m_cmdPoolsMap.end())
					{
						vulkan::CommandPool new_cmd_pool = m_pLogicalDevice->createCommandPool();
						m_cmdPoolsMap.insert(std::make_pair(thread_id, new_cmd_pool));
						pool_it = m_cmdPoolsMap.find(thread_id);
					}
					return pool_it->second;
				}
			public:
				VulkanGuiRenderSystem()
				{
					m_pLogicalDevice = vulkan::VulkanSystem::getLogicalDevice();

					std::array<VkDescriptorSetLayoutBinding, 1> setLayoutBindings = {
						vulkan::vkt::descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT),
					};
					m_imageDescriptorSetLayout = m_pLogicalDevice->createDescriptorSetLayout(setLayoutBindings.data(), setLayoutBindings.size());

					VkDescriptorPoolSize poolSizes = {};
					poolSizes.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					poolSizes.descriptorCount = MAX_IMAGE_COUNT;
					m_imageDescriptorPool = m_pLogicalDevice->createDescriptorPool(&poolSizes, 1, MAX_IMAGE_COUNT);
					m_sampler = m_pLogicalDevice->createSampler();
					m_sdf_sampler = m_pLogicalDevice->createSampler(32, 16, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
					
				}

				virtual ~VulkanGuiRenderSystem()
				{
					m_pLogicalDevice->destroySampler(m_sampler);
					m_pLogicalDevice->destroySampler(m_sdf_sampler);
					//std::cout << "destroy" << std::endl;
					m_pLogicalDevice->destroyDescriptorSetLayout(m_imageDescriptorSetLayout);
					m_pLogicalDevice->destroy(m_imageDescriptorPool);
					for (auto& it : m_cmdPoolsMap)
						m_pLogicalDevice->destroy(it.second);
					m_cmdPoolsMap.clear();
				}

				virtual HandleImage createImage(const void* data, uint32_t size, uint32_t width, uint32_t height) override
				{
					auto pool = getCommandPool();
					Image image;
					m_imageDescriptorPool.create(&m_imageDescriptorSetLayout, &image.set, 1);
					if (size == width * height * 4)
						image.image = m_pLogicalDevice->createTexture2D(pool, width, height, vulkan::ImageFormat::format_rgba8_unorm, data, size);
					else if (size == width * height)
						image.image = m_pLogicalDevice->createTexture2D(pool, width, height, vulkan::ImageFormat::format_r8_unorm, data, size);
					else
						throw std::runtime_error("unrecognized format!");
					VkDescriptorImageInfo imageInfo = vulkan::vkt::descriptorImageInfo(size == width * height ? m_sdf_sampler : m_sampler, image.image.mView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
					VkWriteDescriptorSet bufferWrite = {};
					bufferWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					bufferWrite.dstSet = image.set;
					bufferWrite.dstBinding = 0;
					bufferWrite.dstArrayElement = 0;
					bufferWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					bufferWrite.descriptorCount = 1;
					bufferWrite.pImageInfo = &imageInfo;
					m_pLogicalDevice->updateDescriptorSets(1, &bufferWrite, 0, nullptr);
					auto himage = m_imagePool.store(image);
					return HandleImage(himage);
				}
				virtual void deleteImage(HandleImage himage) override
				{
					if (himage != nullptr)
					{
						Image* pimage = (Image*)himage.operator void* ();
						m_imageDescriptorPool.destroy(&pimage->set, 1);
						m_pLogicalDevice->destroy(pimage->image);
						this->m_imagePool.free(pimage);
					}
					else
						throw std::runtime_error("himage == nullptr!");
				}
				virtual void updateImage(HandleImage image, const void* data, uint32_t size, uint32_t width, uint32_t height) override
				{
					if (image != nullptr)
					{
						auto pool = getCommandPool();
						Image* pimage = (Image*)image.operator void* ();
						if (pimage->image.mWidth == width && pimage->image.mHeight == height)
						{
							m_pLogicalDevice->updateImage2D(pool, &pimage->image, width, height, data, size, vulkan::ImageFormat::format_rgba8_unorm);
						}
						else
						{
							m_pLogicalDevice->destroy(pimage->image);
							pimage->image = m_pLogicalDevice->createTexture2D(pool, width, height, vulkan::ImageFormat::format_rgba8_unorm, data, size);
							VkDescriptorImageInfo imageInfo = vulkan::vkt::descriptorImageInfo(m_sampler, pimage->image.mView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
							VkWriteDescriptorSet bufferWrite = {};
							bufferWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
							bufferWrite.dstSet = pimage->set;
							bufferWrite.dstBinding = 0;
							bufferWrite.dstArrayElement = 0;
							bufferWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
							bufferWrite.descriptorCount = 1;
							bufferWrite.pImageInfo = &imageInfo;
							m_pLogicalDevice->updateDescriptorSets(1, &bufferWrite, 0, nullptr);
						}
					}
					else
						throw std::runtime_error("himage == nullptr!");
				}

				virtual ImageInfo getImageInfo(HandleImage image) override
				{
					ImageInfo info{};
					Image* pimage = (Image*)image.operator void* ();
					if (image == nullptr)
						return info;
					info.mWidth = pimage->image.mWidth;
					info.mHeight = pimage->image.mHeight;
					info.mChannels = pimage->image.mFormat == (int)vulkan::ImageFormat::format_rgba8_unorm ? 4 : 1;
					info.mMipLevels = pimage->image.mMipLevel;
					return info;
				}

				virtual AbstractGuiCanvas* createGuiCanvas(const Size& size) override
				{
					VulkanGuiCanvas* canvas = new VulkanGuiCanvas;
					canvas->init(size.x, size.y, m_pLogicalDevice);
					return canvas;
				}
				virtual void deleteGuiCanvas(AbstractGuiCanvas* canvas) override
				{
					((VulkanGuiCanvas*)canvas)->clear();
					delete canvas;
				}

			};


		}


	};



}
