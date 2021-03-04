#pragma once
#ifndef CRAFT_ENGINE_GRAPHICS_DEBUG_RENDERER_H_
#define CRAFT_ENGINE_GRAPHICS_DEBUG_RENDERER_H_
#include "../Common.h"

#include "../../advance/vulkan/VulkanSystem.h"


namespace CraftEngine
{
	namespace graphics
	{
	

		class DebugRenderer:public AbstractRenderer
		{
		public:
			// Í¨¹ý AbstractRenderer ¼Ì³Ð
			virtual void init(int width, int height, Camera* camera) override
			{

			}
			virtual void updateUniformBuffers() override
			{

			}
			virtual void updateTransform(int id, math::mat4 m) override
			{

			}
			virtual int createModel(HandleModel model_handle) override
			{

			}
			virtual void createInstance(int id) override
			{

			}
			virtual void callDraw() override
			{

			}
			virtual VkImageView getView() override
			{

			}
		};


	}
}





#endif // !CRAFT_ENGINE_GRAPHICS_DEBUG_RENDERER_H_
