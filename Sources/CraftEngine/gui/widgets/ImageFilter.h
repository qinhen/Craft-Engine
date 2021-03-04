#pragma once
#ifndef CRAFT_ENGINE_GUI_IMAGE_FILTER_H_
#define CRAFT_ENGINE_GUI_IMAGE_FILTER_H_
#include "../Vulkan/VulkanImageCompute.h"
#include "./GuiCore.h"
#include "./VulkanGuiRender.h"
namespace CraftEngine
{
	namespace Gui
	{


		class EdgeDetectFilter :CraftEngine::Util::NonCopyable
		{
		private:
			CraftEngine::Vulkan::EdgeDetectPipeline m_pipeline;
			HandleImage m_himage = HandleImage(nullptr);
			VulkanGuiRenderer* m_vkGuiRenderer;
			VulkanGuiRenderer::Image m_output = {};
		public:


			void init(GuiRenderer* renderer)
			{
				m_vkGuiRenderer = (VulkanGuiRenderer*)renderer;
				m_pipeline.init(m_vkGuiRenderer->getLogicalDevice());
				m_output.set = m_vkGuiRenderer->allocImageSet();
			}


			HandleImage getOutputImage(const HandleImage& input)
			{
				VulkanGuiRenderer::Image* pimage = (VulkanGuiRenderer::Image*)(input.operator void* ());
				m_pipeline.setSize(pimage->image.mWidth, pimage->image.mHeight);
				m_pipeline.submit(pimage->image);
				auto output = m_pipeline.getOutput();
				if (output->mImage != m_output.image.mImage)
				{
					m_output.image = *output;
					m_vkGuiRenderer->bindImageSet(m_output.set, *output);
				}
				return HandleImage(&m_output);
			}

		};

		

		class EmbossFilter :CraftEngine::Util::NonCopyable
		{
		private:
			CraftEngine::Vulkan::EmbossPipeline m_pipeline;
			HandleImage m_himage = HandleImage(nullptr);
			VulkanGuiRenderer* m_vkGuiRenderer;
			VulkanGuiRenderer::Image m_output = {};
		public:


			void init(GuiRenderer* renderer)
			{
				m_vkGuiRenderer = (VulkanGuiRenderer*)renderer;
				m_pipeline.init(m_vkGuiRenderer->getLogicalDevice());
				m_output.set = m_vkGuiRenderer->allocImageSet();
			}


			HandleImage getOutputImage(const HandleImage& input)
			{
				VulkanGuiRenderer::Image* pimage = (VulkanGuiRenderer::Image*)(input.operator void* ());
				m_pipeline.setSize(pimage->image.mWidth, pimage->image.mHeight);
				m_pipeline.submit(pimage->image);
				auto output = m_pipeline.getOutput();
				if (output->mImage != m_output.image.mImage)
				{
					m_output.image = *output;
					m_vkGuiRenderer->bindImageSet(m_output.set, *output);
				}
				return HandleImage(&m_output);
			}
		};


		class SharpenFilter :CraftEngine::Util::NonCopyable
		{
		private:
			CraftEngine::Vulkan::SharpenPipeline m_pipeline;
			HandleImage m_himage = HandleImage(nullptr);
			VulkanGuiRenderer* m_vkGuiRenderer;
			VulkanGuiRenderer::Image m_output = {};
		public:


			void init(GuiRenderer* renderer)
			{
				m_vkGuiRenderer = (VulkanGuiRenderer*)renderer;
				m_pipeline.init(m_vkGuiRenderer->getLogicalDevice());
				m_output.set = m_vkGuiRenderer->allocImageSet();
			}


			HandleImage getOutputImage(const HandleImage& input)
			{
				VulkanGuiRenderer::Image* pimage = (VulkanGuiRenderer::Image*)(input.operator void* ());
				m_pipeline.setSize(pimage->image.mWidth, pimage->image.mHeight);
				m_pipeline.submit(pimage->image);
				auto output = m_pipeline.getOutput();
				if (output->mImage != m_output.image.mImage)
				{
					m_output.image = *output;
					m_vkGuiRenderer->bindImageSet(m_output.set, *output);
				}
				return HandleImage(&m_output);
			}
		};






	}
}
#endif