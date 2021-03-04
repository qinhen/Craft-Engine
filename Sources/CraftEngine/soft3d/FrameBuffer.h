#pragma once
#include "./Image.h"

namespace CraftEngine
{
	namespace soft3d
	{

		namespace detail
		{
			constexpr int MaxFrameBufferColorTargetCount = 16;

			struct FrameBufferData
			{
				Image mColorTargets[MaxFrameBufferColorTargetCount];
				Image mDepthStencilTarget;
				int mColorTargetCount;
			};
		}

		class FrameBuffer
		{
		private:
			detail::FrameBufferData* m_framebufferData;
		public:
			FrameBuffer(void* handle) : m_framebufferData((detail::FrameBufferData*)handle) {}
			FrameBuffer() : m_framebufferData(nullptr) {}

			uint32_t bindingCount() const
			{
				return m_framebufferData->mColorTargetCount;
			}
			void clearBindings()
			{
				m_framebufferData->mColorTargetCount = 0;
			}
			int bindColorTarget(const Image& target)
			{
				int index = m_framebufferData->mColorTargetCount;
				if (index >= detail::MaxFrameBufferColorTargetCount)
					soft3d_throw_error(ErrorType::eFrameBufferColorTargetOutOfRange);
				m_framebufferData->mColorTargets[index] = target;
				m_framebufferData->mColorTargetCount++;
				return index;
			}
			void bindDepthTarget(const Image& target)
			{
				m_framebufferData->mDepthStencilTarget = target;
			}

			void* handle() const { return m_framebufferData; }
			bool  valid() const { return handle() != nullptr; }
		};


		FrameBuffer createFrameBuffer(const MemAllocator& allocator = MemAllocator())
		{
			auto framebuffer_data = (detail::FrameBufferData*)allocator.alloc(sizeof(detail::FrameBufferData));
			framebuffer_data->mColorTargetCount = 0;
			return FrameBuffer(framebuffer_data);
		}
		void destroyFrameBuffer(const FrameBuffer& shader, const MemAllocator& allocator = MemAllocator())
		{
			allocator.free(shader.handle());
		}


	}
}