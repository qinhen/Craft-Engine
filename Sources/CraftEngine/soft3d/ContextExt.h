#pragma once
#include "./Context.h"


namespace CraftEngine
{
	namespace soft3d
	{





		class ContextExt
		{
		private:
			Context m_context;
			friend Context createContext();
			friend void destroyContext(Context& context);
		public:
			ContextExt(const Context& contex) :m_context(contex) { }
			ContextExt() :m_context(nullptr) { }

		public:
			void bindPipeline(const Pipeline& pipeline)
			{
				m_context.bindPipeline(pipeline);
			}
			void bindFrameBuffer(const FrameBuffer& framebuffer)
			{
				m_context.bindFrameBuffer(framebuffer);
			}
			void bindSampler(const Sampler& sampler, uint32_t index)
			{
				m_context.bindSampler(sampler, index);
			}
			void bindTexture(const Image& texture, uint32_t index)
			{
				m_context.bindTexture(texture, index);
			}
			void bindBuffer(const Buffer& buffer, uint32_t index)
			{
				m_context.bindBuffer(buffer, index);
			}
			void bindUserData(const void* data, uint32_t index)
			{
				m_context.bindUserData(data, index);
			}
		public:

			void bindVertexBuffer(const Buffer& vertexBuf)
			{
				m_context.bindVertexBuffer(vertexBuf);
			}

			void bindIndexBuffer(const Buffer& indexBuf, ContextInputIndexType indexType = ContextInputIndexType::eUInt32)
			{
				m_context.bindIndexBuffer(indexBuf, indexType);
			}


			void drawRect(const ivec2& pMin, const ivec2& pMax, vec4 color)
			{
				detail::ImagePixelData pixelData;
				auto context = (detail::ContextData*)handle();

				auto width = context->mCurFrameBuffer.m_framebufferData->mColorTargets[0].width();
				auto height = context->mCurFrameBuffer.m_framebufferData->mColorTargets[0].height();
				if (output.mFragPos.x < 0 || output.mFragPos.x >= width)
					return;
				if (output.mFragPos.y < 0 || output.mFragPos.y >= height)
					return;
				auto pixel_index = width * output.mFragPos.y + output.mFragPos.x;
				detail::ImagePixelData pixel_data;
				for (int i = 0; i < m_contextData->mCurPipeline.m_pipelineData->mAttachmentCount; i++)
				{
					auto ptr = (byte*)m_contextData->mCurFrameBuffer.m_framebufferData->mColorTargets[0].m_imageData->mMappedPtr;
					auto& pixel_size = m_contextData->mCurFrameBuffer.m_framebufferData->mColorTargets[0].m_imageData->mPixelBytes;
					ptr = ptr + pixel_index * pixel_size;
					pixel_data = detail::castVectorToPixel(output.mColors[i], m_contextData->mCurFrameBuffer.m_framebufferData->mColorTargets[0].format());
					memcpy(ptr, &pixel_data, pixel_size);
				}




			}









			void* handle() const { return m_context.handle(); }
			bool  valid() const { return handle() != nullptr; }

		private:







		};




		Context createContext()
		{		
			auto ctx_data = new detail::ContextData;
			Context context(ctx_data);
			ctx_data->mAllocator = createMemoryAllocator();
			return context;
		}

		void destroyContext(Context& context)
		{
			auto ctx_data = (detail::ContextData*)context.handle();
			soft3d::destroyMemoryAllocator(ctx_data->mAllocator);
			delete ctx_data;
			context.m_contextData = nullptr;
		}
		



	}
}


namespace CraftEngine
{
	namespace soft3d
	{



	}
}