#pragma once
#include "./Image.h"

namespace CraftEngine
{
	namespace soft3d
	{

		

		class FrameBuffer
		{
		private:
			detail::FramebufferData* m_framebufferData;
			friend class Context;
		public:
			FrameBuffer(void* handle) : m_framebufferData((detail::FramebufferData*)handle) {}
			FrameBuffer() : m_framebufferData(nullptr) {}

			void bindColorTarget(const Image& target, int index)
			{
				m_framebufferData->mColorTargets[index] = (detail::ImageData*)target.handle();
				m_framebufferData->mColorTargetCount++;
			}
			void bindDepthTarget(const Image& target)
			{
				m_framebufferData->mDepthStencilTarget = (detail::ImageData*)target.handle();
			}

			void* handle() const { return m_framebufferData; }
			bool  valid() const { return handle() != nullptr; }
		};


	}
}