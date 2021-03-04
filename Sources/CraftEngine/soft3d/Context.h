#pragma once
#include "./Common.h"

#include "./Memory.h"
#include "./Buffer.h"
#include "./Image.h"
#include "./ImageView.h"
#include "./Sampler.h"
#include "./SamplerExt.h"
#include "./ImageOps.h"
#include "./BufferOps.h"

#include "./FrameBuffer.h"
#include "./Shader.h"
#include "./Pipeline.h"
#include "./Device.h"

namespace CraftEngine
{
	namespace soft3d
	{

		struct IRect
		{
			ivec2 mOffset;
			ivec2 mSize;
		};

		typedef IRect Viewport;
		typedef IRect Scissor;

		namespace detail
		{
			struct GraphicsContextData
			{
				MemAllocator            mAllocator;
				Device                  mDevice;

				Buffer                  mCurVertexBuffer;
				Buffer                  mCurIndexBuffer;
				IndexType               mCurIndexType;

				vec2                    mCurTargetSize;
				Viewport                mCurViewport;	
				Scissor                 mCurScissor;

				Pipeline                mCurPipeline;
				FrameBuffer             mCurFrameBuffer;
				ShaderResources         mTempRenderResources;

				int32_t                 mTempVertexCount;
				ShaderVertexPhaseOutput mTempVertexList[4];
				ShaderVertexPhaseOutput mTempClipVertexList[6];
			};
		}


		class GraphicsContext
		{
		private:
			detail::GraphicsContextData* m_contextData;
		public:
			GraphicsContext(void* handle) :m_contextData((detail::GraphicsContextData*)handle) { }
			GraphicsContext() :m_contextData(nullptr) { }

			MemAllocator& memAllocator() { return m_contextData->mAllocator; };
		public:
			void bindPipeline(const Pipeline& pipeline)
			{
				m_contextData->mCurPipeline = pipeline;
			}
			void bindFrameBuffer(const FrameBuffer& framebuffer)
			{
				m_contextData->mCurFrameBuffer = framebuffer;
				m_contextData->mCurTargetSize.x = ((detail::FrameBufferData*)m_contextData->mCurFrameBuffer.handle())->mColorTargets[0].width();
				m_contextData->mCurTargetSize.y = ((detail::FrameBufferData*)m_contextData->mCurFrameBuffer.handle())->mColorTargets[0].height();
			}
			void bindSampler(const Sampler& sampler, uint32_t index)
			{
				m_contextData->mTempRenderResources.mSamplers[index] = sampler;
			}
			void bindTexture(const Image& texture, uint32_t index)
			{
				m_contextData->mTempRenderResources.mImages[index] = texture;
			}
			void bindBuffer(const Buffer& buffer, uint32_t index)
			{
				m_contextData->mTempRenderResources.mBuffers[index] = buffer;
			}
			void bindUserData(const void* data, uint32_t index)
			{
				m_contextData->mTempRenderResources.mUserDatas[index] = data;
			}
			void pushConstants(const void* data, size_t count, size_t offset = 0)
			{
				memcpy(m_contextData->mTempRenderResources.mPushContants + offset, data, count);
			}

		public:
			void bindVertexBuffer(const Buffer& vertexBuf)
			{
				m_contextData->mCurVertexBuffer = vertexBuf;
			}
			void bindIndexBuffer(const Buffer& indexBuf, IndexType indexType = IndexType::eUInt32)
			{
				m_contextData->mCurIndexBuffer = indexBuf;
				m_contextData->mCurIndexType = indexType;
			}
		public:

			void setScissor(const Scissor& scissor)
			{
				m_contextData->mCurScissor = scissor;
			}

			void setViewport(const Viewport& viewport)
			{
				m_contextData->mCurViewport = viewport;
			}
		public:

			void enableNoBlock(bool enable)
			{
				auto device = m_contextData->mDevice;
				auto device_data = (detail::DeviceData*)device.handle();
				device_data->mNoBlock = enable;
				if (enable == true)
					device.waitDevice();
			}

			void drawVertex(
				uint32_t vertexCount,
				uint32_t instanceCount,
				uint32_t firstVertex,
				uint32_t firstInstance
			);

			void drawIndex(
				uint32_t indexCount,
				uint32_t instanceCount,
				uint32_t firstIndex,
				int32_t vertexOffset,
				uint32_t firstInstance
			);

			void* handle() const { return m_contextData; }
			bool  valid() const { return handle() != nullptr; }

		private:

			// 发送顶点
			void sendVertex(ShaderVertexPhaseOutput& shader_vertex_output);

			// 存入片段计算结果
			void acceptFragment(const Pipeline& pipeline, const ShaderFragmentPhaseOutput& output, const ivec2& fragPos, float depth, const Scissor& scissor);

			// 片元坐标归一化，顶点输出属性归一化
			void homogenizeVertexShaderVaryingOutput(ShaderVertexPhaseOutput& vertex);

			// 面剔除
			bool cullFace();

			// 点裁剪
			int clipPoint(const ShaderVertexPhaseOutput& p1);

			// 线段裁剪
			int clipLine(const ShaderVertexPhaseOutput& p1, const ShaderVertexPhaseOutput& p2);

			// 三角形裁剪
			int clipTriangle(const ShaderVertexPhaseOutput p[3]);

			// 绘制点
			void drawPoint(const ShaderVertexPhaseOutput& p1);

			// 绘制线段
			void drawLine(const ShaderVertexPhaseOutput& p1, const ShaderVertexPhaseOutput& p2);

			// 绘制三角形
			void drawTriganle(const ShaderVertexPhaseOutput p[3]);

			// 光栅化点
			void rasterizePoint(const ShaderVertexPhaseOutput& input);

			// 光栅化线段
			void rasterizeLine(const ShaderVertexPhaseOutput& p1, const ShaderVertexPhaseOutput& p2);

			// 光栅化三角形
			void rasterizeTriganle(const ShaderVertexPhaseOutput& p1, const ShaderVertexPhaseOutput& p2, const ShaderVertexPhaseOutput& p3);

		};




		GraphicsContext createGraphicsContext(const Device& device, const MemAllocator& allocator = MemAllocator())
		{
			auto ctx_data = (detail::GraphicsContextData*)allocator.alloc(sizeof(detail::GraphicsContextData));
			GraphicsContext context(ctx_data);
			ctx_data->mAllocator = allocator;
			ctx_data->mDevice = device;
			return context;
		}

		void destroyGraphicsContext(GraphicsContext& context, const MemAllocator& allocator = MemAllocator())
		{
			auto ctx_data = (detail::GraphicsContextData*)context.handle();
			delete ctx_data;
		}




	}
}


namespace CraftEngine
{
	namespace soft3d
	{



		void GraphicsContext::drawVertex(
			uint32_t vertexCount,
			uint32_t instanceCount,
			uint32_t firstVertex,
			uint32_t firstInstance
		)
		{
			m_contextData->mTempVertexCount = 0;

			auto framebuffer_data = (detail::FrameBufferData*)m_contextData->mCurFrameBuffer.handle();
			auto pipeline_data = (detail::PipelineData*)m_contextData->mCurPipeline.handle();
			auto vertex_stride = pipeline_data->mVertexStride;
			auto vertex_data = (byte*)m_contextData->mCurVertexBuffer.data();

			auto shader_data = (detail::ShaderData*)pipeline_data->mShader.handle();
			auto& resource_data = m_contextData->mTempRenderResources;

			ShaderVertexPhaseInput shader_vertex_input;
			ShaderVertexPhaseOutput shader_vertex_output;
			int v = 0;
			for (int i = firstInstance; i < firstInstance + instanceCount; i++)
			{
				shader_vertex_input.mInstance = i;
				shader_vertex_input.mVertex = firstVertex;
				shader_vertex_input.mIndex = 0;

				for (int j = 0; j < vertexCount; j++)
				{
					shader_vertex_input.mAttributes = vertex_data;
					shader_data->mVertexShader(resource_data, shader_vertex_input, shader_vertex_output);
					sendVertex(shader_vertex_output);
					vertex_data += vertex_stride;
					shader_vertex_input.mVertex++;
					shader_vertex_input.mIndex++;
				}
			}
		}

		void GraphicsContext::drawIndex(
			uint32_t indexCount,
			uint32_t instanceCount,
			uint32_t firstIndex,
			int32_t  vertexOffset,
			uint32_t firstInstance
		)
		{
			m_contextData->mTempVertexCount = 0;

			auto framebuffer_data = (detail::FrameBufferData*)m_contextData->mCurFrameBuffer.handle();
			auto pipeline_data = (detail::PipelineData*)m_contextData->mCurPipeline.handle();
			int32_t vertex_stride = pipeline_data->mVertexStride;
			const auto vertex_data = ((byte*)m_contextData->mCurVertexBuffer.data()) + vertexOffset * vertex_stride;

			auto index_data = (byte*)m_contextData->mCurIndexBuffer.data();

			auto shader_data = (detail::ShaderData*)pipeline_data->mShader.handle();
			auto& resource_data = m_contextData->mTempRenderResources;

			ShaderVertexPhaseInput shader_vertex_input;
			ShaderVertexPhaseOutput shader_vertex_output;
			uint32_t index = 0;

			switch (m_contextData->mCurIndexType)
			{
			case IndexType::eUInt32:
				for (int i = firstInstance; i < firstInstance + instanceCount; i++)
				{
					shader_vertex_input.mInstance = i;
					shader_vertex_input.mVertex = 0;
					shader_vertex_input.mIndex = firstIndex;

					for (int j = 0; j < indexCount; j++)
					{
						index = ((uint32_t*)index_data)[shader_vertex_input.mIndex];
						shader_vertex_input.mAttributes = vertex_data + vertex_stride * index;
						shader_data->mVertexShader(resource_data, shader_vertex_input, shader_vertex_output);
						sendVertex(shader_vertex_output);
						shader_vertex_input.mVertex++;
						shader_vertex_input.mIndex++;
					}
				}
				break;
			case IndexType::eUInt16:
				for (int i = firstInstance; i < firstInstance + instanceCount; i++)
				{
					shader_vertex_input.mInstance = i;
					shader_vertex_input.mVertex = 0;
					shader_vertex_input.mIndex = firstIndex;

					for (int j = 0; j < indexCount; j++)
					{
						index = ((uint16_t*)index_data)[shader_vertex_input.mIndex];
						shader_vertex_input.mAttributes = vertex_data + vertex_stride * index;
						shader_data->mVertexShader(resource_data, shader_vertex_input, shader_vertex_output);
						sendVertex(shader_vertex_output);
						shader_vertex_input.mVertex++;
						shader_vertex_input.mIndex++;
					}
				}
				break;
			case IndexType::eUInt8:
				for (int i = firstInstance; i < firstInstance + instanceCount; i++)
				{
					shader_vertex_input.mInstance = i;
					shader_vertex_input.mVertex = 0;
					shader_vertex_input.mIndex = firstIndex;

					for (int j = 0; j < indexCount; j++)
					{
						index = ((uint8_t*)index_data)[shader_vertex_input.mIndex];
						shader_vertex_input.mAttributes = vertex_data + vertex_stride * index;
						shader_data->mVertexShader(resource_data, shader_vertex_input, shader_vertex_output);
						sendVertex(shader_vertex_output);
						shader_vertex_input.mVertex++;
						shader_vertex_input.mIndex++;
					}
				}
				break;
			}

		}

		inline void GraphicsContext::sendVertex(ShaderVertexPhaseOutput& shader_vertex_output)
		{
			homogenizeVertexShaderVaryingOutput(shader_vertex_output);
			auto pipeline_data = (detail::PipelineData*)m_contextData->mCurPipeline.handle();
			switch (pipeline_data->mPolygonType)
			{
			case PipelinePolygonType::eTriangleList:
			{
				m_contextData->mTempVertexList[m_contextData->mTempVertexCount] = shader_vertex_output;
				m_contextData->mTempVertexCount++;
				if (m_contextData->mTempVertexCount == 3)
				{
					if (cullFace())
					{
						if (pipeline_data->mPolygonMode == PipelinePolygonMode::eFillMode)
							drawTriganle(m_contextData->mTempVertexList);
						else if (pipeline_data->mPolygonMode == PipelinePolygonMode::ePointMode)
						{
							drawPoint(m_contextData->mTempVertexList[0]);
							drawPoint(m_contextData->mTempVertexList[1]);
							drawPoint(m_contextData->mTempVertexList[2]);
						}
						else
						{
							drawLine(m_contextData->mTempVertexList[0], m_contextData->mTempVertexList[1]);
							drawLine(m_contextData->mTempVertexList[1], m_contextData->mTempVertexList[2]);
							drawLine(m_contextData->mTempVertexList[2], m_contextData->mTempVertexList[0]);
						}
					}
					m_contextData->mTempVertexCount = 0;
				}
				break;
			}
			case PipelinePolygonType::eLineList:
			{
				m_contextData->mTempVertexList[m_contextData->mTempVertexCount] = shader_vertex_output;
				m_contextData->mTempVertexCount++;
				if (m_contextData->mTempVertexCount == 2)
				{
					drawLine(m_contextData->mTempVertexList[0], m_contextData->mTempVertexList[1]);
					m_contextData->mTempVertexCount = 0;
				}
				break;
			}
			case PipelinePolygonType::eLineStrip:
			{
				switch (m_contextData->mTempVertexCount)
				{
				case 0:
					m_contextData->mTempVertexList[0] = shader_vertex_output;
					m_contextData->mTempVertexCount++;
					break;
				case 1:
					m_contextData->mTempVertexList[1] = shader_vertex_output;
					drawLine(m_contextData->mTempVertexList[0], m_contextData->mTempVertexList[1]);
					m_contextData->mTempVertexList[0] = shader_vertex_output;
					break;
				}
				break;
			}
			case PipelinePolygonType::ePointList:
			{
				m_contextData->mTempVertexList[0] = shader_vertex_output;
				drawPoint(m_contextData->mTempVertexList[0]);
				break;
			}
			case PipelinePolygonType::eTriangleStrip:
			{
				switch (m_contextData->mTempVertexCount)
				{
				case 3:
					m_contextData->mTempVertexList[2] = shader_vertex_output;
					if (cullFace())
					{
						if (pipeline_data->mPolygonMode == PipelinePolygonMode::eFillMode)
							drawTriganle(m_contextData->mTempVertexList);
						else if (pipeline_data->mPolygonMode == PipelinePolygonMode::ePointMode)
						{
							drawPoint(m_contextData->mTempVertexList[0]);
							drawPoint(m_contextData->mTempVertexList[1]);
							drawPoint(m_contextData->mTempVertexList[2]);
						}
						else
						{
							drawLine(m_contextData->mTempVertexList[0], m_contextData->mTempVertexList[1]);
							drawLine(m_contextData->mTempVertexList[1], m_contextData->mTempVertexList[2]);
							drawLine(m_contextData->mTempVertexList[2], m_contextData->mTempVertexList[0]);
						}
					}
					m_contextData->mTempVertexList[0] = m_contextData->mTempVertexList[1];
					m_contextData->mTempVertexList[1] = m_contextData->mTempVertexList[2];
					break;
				case 0:
				case 1:
					m_contextData->mTempVertexList[0] = shader_vertex_output;
					m_contextData->mTempVertexCount++;
					break;
				case 2:
					m_contextData->mTempVertexList[1] = shader_vertex_output;
					m_contextData->mTempVertexCount++;
					break;
				}
				break;
			}
			case PipelinePolygonType::eTriangleFan:
			case PipelinePolygonType::ePolygon:
			{
				switch (m_contextData->mTempVertexCount)
				{
				case 2:
					m_contextData->mTempVertexList[2] = shader_vertex_output;
					if (cullFace())
					{
						if (pipeline_data->mPolygonMode == PipelinePolygonMode::eFillMode)
							drawTriganle(m_contextData->mTempVertexList);
						else if (pipeline_data->mPolygonMode == PipelinePolygonMode::ePointMode)
						{
							drawPoint(m_contextData->mTempVertexList[0]);
							drawPoint(m_contextData->mTempVertexList[1]);
							drawPoint(m_contextData->mTempVertexList[2]);
						}
						else
						{
							drawLine(m_contextData->mTempVertexList[0], m_contextData->mTempVertexList[1]);
							drawLine(m_contextData->mTempVertexList[1], m_contextData->mTempVertexList[2]);
							drawLine(m_contextData->mTempVertexList[2], m_contextData->mTempVertexList[0]);
						}
					}
					m_contextData->mTempVertexList[1] = m_contextData->mTempVertexList[2];
					break;
				case 0:
					m_contextData->mTempVertexList[0] = shader_vertex_output;
					m_contextData->mTempVertexCount++;
					break;
				case 1:
					m_contextData->mTempVertexList[1] = shader_vertex_output;
					m_contextData->mTempVertexCount++;
					break;
				}
				break;
			}
			}
		}

		inline void GraphicsContext::acceptFragment(const Pipeline& pipeline, const ShaderFragmentPhaseOutput& output, const ivec2& fragPos, float depth, const Scissor& scissor)
		{
			// depth cull
			if (depth > 1.0f || depth < 0.0f)
				return;

			int width = m_contextData->mCurTargetSize.x;
			int height = m_contextData->mCurTargetSize.y;

			if (fragPos.x < 0 || fragPos.x >= width)
				return;
			if (fragPos.y < 0 || fragPos.y >= height)
				return;

			auto pixel_index = width * fragPos.y + fragPos.x;
			auto pipeline_data = (detail::PipelineData*)pipeline.handle();

			// scissor test
			if (pipeline_data->mEnableScissorTest)
			{
				if (fragPos.x < scissor.mOffset.x ||
					fragPos.y < scissor.mOffset.y ||
					fragPos.x >= scissor.mOffset.x + scissor.mSize.x ||
					fragPos.y >= scissor.mOffset.y + scissor.mSize.y
					)return;
			}

			// depth test
			auto& depth_stencil_buffer = ((detail::FrameBufferData*)m_contextData->mCurFrameBuffer.handle())->mDepthStencilTarget;
			if (pipeline_data->mEnableDepthTest &&
				depth_stencil_buffer.valid())
			{
				auto ptr = (byte*)((detail::ImageData*)depth_stencil_buffer.handle())->mMappedPtr;
				auto& pixel_size = ((detail::ImageData*)depth_stencil_buffer.handle())->mPixelBytes;
				ptr = ptr + pixel_index * pixel_size;
				auto pixel = (detail::ImagePixelData*)ptr;
				float dst_depth;
				switch (depth_stencil_buffer.format())
				{
				case ImageFormat::eD24_UNORM_S8_UINT:
					dst_depth = pixel->mD24_UNORM_S8_UINT.D24 * (1.0f / ((1 << 24) - 1));
					break;
				case ImageFormat::eD32_SFLOAT:
					dst_depth = pixel->mD32_SFLOAT;
					break;
				}
				switch (pipeline_data->mDepthCompareMode)
				{
				case PipelineDepthCompareMode::eLessEqualMode:
					if (depth > dst_depth)
						return;
					break;
				case PipelineDepthCompareMode::eLessMode:
					if (depth >= dst_depth)
						return;
					break;
				case PipelineDepthCompareMode::eGreaterEqualMode:
					if (depth < dst_depth)
						return;
					break;
				case PipelineDepthCompareMode::eGreaterMode:
					if (depth <= dst_depth)
						return;
					break;
				case PipelineDepthCompareMode::eEqualMode:
					if (depth != dst_depth)
						return;
					break;
				}
				if (pipeline_data->mEnableDepthWrite)
				{
					switch (depth_stencil_buffer.format())
					{
					case ImageFormat::eD24_UNORM_S8_UINT:
						pixel->mD24_UNORM_S8_UINT.D24 = depth * ((1 << 24) - 1);
						break;
					case ImageFormat::eD32_SFLOAT:
						pixel->mD32_SFLOAT = depth;
						break;
					}
				}
			}

			detail::ImagePixelData pixel_data;
			Image* color_target_list = ((detail::FrameBufferData*)m_contextData->mCurFrameBuffer.handle())->mColorTargets;
			if (pipeline_data->mEnableColorBlend)
			{
				for (int i = 0; i < pipeline_data->mAttachmentCount; i++)
				{
					auto ptr = (byte*)((detail::ImageData*)color_target_list[i].handle())->mMappedPtr;
					auto& pixel_size = ((detail::ImageData*)color_target_list[i].handle())->mPixelBytes;
					ptr = ptr + pixel_index * pixel_size;
					memcpy(&pixel_data, ptr, pixel_size);
					vec4 dst_color = detail::castPixelToVector(pixel_data, color_target_list[i].format());
					vec4 src_color = output.mColors[i].a * output.mColors[i] + (1.0f - output.mColors[i].a) * dst_color;
					pixel_data = detail::castVectorToPixel(src_color, color_target_list[i].format());
					memcpy(ptr, &pixel_data, pixel_size);
				}
			}
			else
			{
				for (int i = 0; i < pipeline_data->mAttachmentCount; i++)
				{
					auto ptr = (byte*)((detail::ImageData*)color_target_list[i].handle())->mMappedPtr;
					auto& pixel_size = ((detail::ImageData*)color_target_list[i].handle())->mPixelBytes;
					ptr = ptr + pixel_index * pixel_size;
					pixel_data = detail::castVectorToPixel(output.mColors[i], color_target_list[i].format());
					memcpy(ptr, &pixel_data, pixel_size);
				}
			}
		}

		inline void GraphicsContext::homogenizeVertexShaderVaryingOutput(ShaderVertexPhaseOutput& vertex)
		{
			vertex.mPosition.w = math::inverse(vertex.mPosition.w);
			vertex.mPosition.xyz *= vertex.mPosition.w;
			vertex.mPosition.xy = (vertex.mPosition.xy + 1.0f) * 0.5f * m_contextData->mCurTargetSize;
			for (int i = 0; i < detail::MaxShaderAttributeCount; i++)
				vertex.mAttributes[i] = vertex.mAttributes[i] * vertex.mPosition.w;
		}

		inline bool GraphicsContext::cullFace()
		{
			auto pipeline_data = (detail::PipelineData*)m_contextData->mCurPipeline.handle();
			auto v1 = math::dvec2(m_contextData->mTempVertexList[1].mPosition.xy - m_contextData->mTempVertexList[0].mPosition.xy);
			auto v2 = math::dvec2(m_contextData->mTempVertexList[2].mPosition.xy - m_contextData->mTempVertexList[0].mPosition.xy);
			auto v = math::cross(v1, v2);
			bool cw = pipeline_data->mFrontFace == PipelineFrontFace::eClockWire;
			switch (pipeline_data->mCullFace)
			{
			case PipelineCullFace::eCullBackFace:
				if (cw)
					return v > 0;
				else
					return v < 0;
			case PipelineCullFace::eCullFrontFace:
				if (cw)
					return v < 0;
				else
					return v > 0;
			case PipelineCullFace::eCullNone:
				return true;
			case PipelineCullFace::eCullFrontAndBackFace:
				return false;
			default:
				return true;
			}
		}

		inline int GraphicsContext::clipPoint(const ShaderVertexPhaseOutput& p1)
		{
			float left = 0, right = m_contextData->mCurTargetSize.x,
				top = 0, bottom = m_contextData->mCurTargetSize.y;
			return (p1.mPosition.x < left || p1.mPosition.x > right ||
				p1.mPosition.y < top || p1.mPosition.y > bottom) ? 0 : 1;
		}

		inline int GraphicsContext::clipLine(const ShaderVertexPhaseOutput& p1, const ShaderVertexPhaseOutput& p2)
		{
			auto pos1 = vec2(p1.mPosition.xy);
			auto pos2 = vec2(p2.mPosition.xy);
			auto tpos1 = pos1;
			auto tpos2 = pos2;
			float left = 0, right = m_contextData->mCurTargetSize.x,
				top = 0, bottom = m_contextData->mCurTargetSize.y;
			auto calPointMask = [&](const vec2& p) {
				int mask = 0;
				if (p.x > right) mask |= 0x1;
				else if (p.x < left) mask |= 0x2;
				if (p.y > bottom) mask |= 0x4;
				else if (p.y < top) mask |= 0x8;
				return mask;
			};

			auto mk1 = calPointMask(p1.mPosition.xy);
			auto mk2 = calPointMask(p2.mPosition.xy);
			if ((mk1 | mk2) == 0) return -1;
			if ((mk1 & mk2) != 0) return 0;

			auto mk1andmk2 = mk1 & mk2;
			if (mk1 & 0x1)
				tpos1 = math::lineIntersectionX(tpos1, tpos2, right);
			else if (mk2 & 0x1)
				tpos2 = math::lineIntersectionX(tpos1, tpos2, right);
			if (mk1 & 0x2)
				tpos1 = math::lineIntersectionX(tpos1, tpos2, left);
			else if (mk2 & 0x2)
				tpos2 = math::lineIntersectionX(tpos1, tpos2, left);
			if (mk1 & 0x4)
				tpos1 = math::lineIntersectionY(tpos1, tpos2, bottom);
			else if (mk2 & 0x4)
				tpos2 = math::lineIntersectionY(tpos1, tpos2, bottom);
			if (mk1 & 0x8)
				tpos1 = math::lineIntersectionY(tpos1, tpos2, top);
			else if (mk2 & 0x8)
				tpos2 = math::lineIntersectionY(tpos1, tpos2, top);

			auto l = math::length(pos1 - pos2);
			auto t1 = math::length(tpos1 - pos1) / l;
			auto t2 = math::length(tpos2 - pos2) / l;
			if (t1 + t2 > 1.0f + 4 * std::numeric_limits<float>::epsilon())
				return 0;

			m_contextData->mTempClipVertexList[0].mPosition = (1 - t1) * p1.mPosition + t1 * p2.mPosition;
			for (int i = 0; i < detail::MaxShaderAttributeCount; i++)
				m_contextData->mTempClipVertexList[0].mAttributes[i] = (1 - t1) * p1.mAttributes[i] + t1 * p2.mAttributes[i];
			m_contextData->mTempClipVertexList[1].mPosition = (1 - t2) * p2.mPosition + t2 * p1.mPosition;
			for (int i = 0; i < detail::MaxShaderAttributeCount; i++)
				m_contextData->mTempClipVertexList[1].mAttributes[i] = (1 - t2) * p2.mAttributes[i] + t2 * p1.mAttributes[i];
			return 1;
		}

		inline int GraphicsContext::clipTriangle(const ShaderVertexPhaseOutput p[3])
		{
			using dvec2 = math::dvec2;
			using dvec3 = math::dvec3;

			const dvec2 pos[3] = {
				dvec2(p[0].mPosition.xy),
				dvec2(p[1].mPosition.xy),
				dvec2(p[2].mPosition.xy) };
			dvec2 temp_pos[2][6];
			double left = 0, right = m_contextData->mCurTargetSize.x,
				top = 0, bottom = m_contextData->mCurTargetSize.y;
			int vertex_count = 3, temp_index = 0, next_index,
				clip_count, reserve_count, first_reserve, last_reserve, masks[6];
			bool is_clip = false;
			temp_pos[temp_index][0] = pos[0];
			temp_pos[temp_index][1] = pos[1];
			temp_pos[temp_index][2] = pos[2];
			for (int d = 0; d < 4; d++)
			{ // clip
				clip_count = 0;
				first_reserve = 0;
				next_index = temp_index == 0 ? 1 : 0;
				for (int i = 0; i < vertex_count; i++)
				{
					if ((d == 0 && temp_pos[temp_index][i].x > right) ||
						(d == 1 && temp_pos[temp_index][i].x < left) ||
						(d == 2 && temp_pos[temp_index][i].y > bottom) ||
						(d == 3 && temp_pos[temp_index][i].y < top)) {
						masks[i] = 1;
						clip_count++;
					}
					else
					{
						masks[i] = 0;
					}
				}
				if (clip_count == vertex_count)
					return 0;
				if (clip_count == 0)
					continue;
				is_clip = true;
				if (masks[0] == 0)
				{
					for (int i = 1; i < vertex_count; i++)
						if (masks[i] == 1)
						{
							first_reserve = i - 1;
							break;
						}
					first_reserve = (first_reserve + clip_count + 1) % vertex_count;;
				}
				else
				{
					for (int i = 1; i < vertex_count; i++)
						if (masks[i] == 0)
						{
							first_reserve = i;
							break;
						}
				}
				reserve_count = vertex_count - clip_count;
				last_reserve = (first_reserve + reserve_count) % vertex_count;

				switch (d)
				{
				case 0:
					temp_pos[next_index][reserve_count + 1] = math::lineIntersectionX(temp_pos[temp_index][(first_reserve - 1 + vertex_count) % vertex_count], temp_pos[temp_index][first_reserve], right);
					temp_pos[next_index][reserve_count] = math::lineIntersectionX(temp_pos[temp_index][(last_reserve - 1 + vertex_count) % vertex_count], temp_pos[temp_index][last_reserve], right);
					break;
				case 1:
					temp_pos[next_index][reserve_count + 1] = math::lineIntersectionX(temp_pos[temp_index][(first_reserve - 1 + vertex_count) % vertex_count], temp_pos[temp_index][first_reserve], left);
					temp_pos[next_index][reserve_count] = math::lineIntersectionX(temp_pos[temp_index][(last_reserve - 1 + vertex_count) % vertex_count], temp_pos[temp_index][last_reserve], left);
					break;
				case 2:
					temp_pos[next_index][reserve_count + 1] = math::lineIntersectionY(temp_pos[temp_index][(first_reserve - 1 + vertex_count) % vertex_count], temp_pos[temp_index][first_reserve], bottom);
					temp_pos[next_index][reserve_count] = math::lineIntersectionY(temp_pos[temp_index][(last_reserve - 1 + vertex_count) % vertex_count], temp_pos[temp_index][last_reserve], bottom);
					break;
				case 3:
					temp_pos[next_index][reserve_count + 1] = math::lineIntersectionY(temp_pos[temp_index][(first_reserve - 1 + vertex_count) % vertex_count], temp_pos[temp_index][first_reserve], top);
					temp_pos[next_index][reserve_count] = math::lineIntersectionY(temp_pos[temp_index][(last_reserve - 1 + vertex_count) % vertex_count], temp_pos[temp_index][last_reserve], top);
					break;
				}
				int j = 0;
				int k = first_reserve;
				for (; j < reserve_count; k++, j++)
					temp_pos[next_index][j] = temp_pos[temp_index][k % vertex_count];
				temp_index = next_index;
				vertex_count = reserve_count + 2;
			}
			if (!is_clip)
				return -1;
			dvec3 t;
			float total = math::abs(math::cross((pos[1] - pos[0]), (pos[2] - pos[0])));

			for (int i = 0; i < vertex_count; i++)
			{
				t[0] = math::abs(math::cross((pos[1] - temp_pos[temp_index][i]), (pos[2] - temp_pos[temp_index][i])));
				t[1] = math::abs(math::cross((pos[0] - temp_pos[temp_index][i]), (pos[2] - temp_pos[temp_index][i])));
				t[2] = math::abs(math::cross((pos[0] - temp_pos[temp_index][i]), (pos[1] - temp_pos[temp_index][i])));
				t /= total;
				m_contextData->mTempClipVertexList[i].mPosition.xy = vec2(temp_pos[temp_index][i]);
				m_contextData->mTempClipVertexList[i].mPosition.z = t[0] * p[0].mPosition.z + t[1] * p[1].mPosition.z + t[2] * p[2].mPosition.z;
				m_contextData->mTempClipVertexList[i].mPosition.w = t[0] * p[0].mPosition.w + t[1] * p[1].mPosition.w + t[2] * p[2].mPosition.w;
				for (int j = 0; j < detail::MaxShaderAttributeCount; j++)
					m_contextData->mTempClipVertexList[i].mAttributes[j] = t[0] * p[0].mAttributes[j] + t[1] * p[1].mAttributes[j] + t[2] * p[2].mAttributes[j];
			}
			return vertex_count;
		}

		inline void GraphicsContext::drawPoint(const ShaderVertexPhaseOutput& p1)
		{
			if (clipPoint(p1) == 1)
				rasterizePoint(p1);
		}

		inline void GraphicsContext::drawLine(const ShaderVertexPhaseOutput& p1, const ShaderVertexPhaseOutput& p2)
		{
			switch (clipLine(p1, p2))
			{
			case -1:
				rasterizeLine(p1, p2);
				break;
			case 1:
				rasterizeLine(m_contextData->mTempClipVertexList[0], m_contextData->mTempClipVertexList[1]);
				break;
			}
		}

		inline void GraphicsContext::drawTriganle(const ShaderVertexPhaseOutput p[3])
		{
			int vertex_count = clipTriangle(p);
			if (vertex_count == 0)
				return;
			if (vertex_count == -1)
				rasterizeTriganle(p[0], p[1], p[2]);
			else
			{
				for (int i = 2; i < vertex_count; i++)
				{
					rasterizeTriganle(
						m_contextData->mTempClipVertexList[0],
						m_contextData->mTempClipVertexList[i - 1],
						m_contextData->mTempClipVertexList[i]);
				}
			}
		}

		inline void GraphicsContext::rasterizePoint(const ShaderVertexPhaseOutput& input)
		{
			auto device = m_contextData->mDevice;
			auto device_data = (detail::DeviceData*)device.handle();
			if (device_data->mNoBlock == false)
				device.waitDevice();
			auto pipeline_data = (detail::PipelineData*)m_contextData->mCurPipeline.handle();
			auto thread_count = device_data->mThreadPool.threadCount();
			auto cur_thread = device_data->mCurThread + 1;
			cur_thread %= thread_count;
			device_data->mThreadPool.push([this, input,
				pipeline = m_contextData->mCurPipeline,
				scissor = m_contextData->mCurScissor,
				fragment_shader = ((detail::ShaderData*)pipeline_data->mShader.handle())->mFragmentShader,
				resource_data = m_contextData->mTempRenderResources]() {
				ShaderFragmentPhaseInput frag_input;
				ShaderFragmentPhaseOutput frag_output;
				frag_input.mFragPos = ivec2(input.mPosition.xy + 0.5f);
				frag_input.mDepth = input.mPosition.z;
				frag_input.mStencil = 0;
				for (int i = 0; i < detail::MaxShaderAttributeCount; i++)
					frag_input.mAttributes[i] = input.mAttributes[i];

				frag_output.mDiscard = false;
				fragment_shader(resource_data, frag_input, frag_output);
				if (frag_output.mDiscard == false)
					acceptFragment(pipeline, frag_output, frag_input.mFragPos, frag_input.mDepth, scissor);
			}, cur_thread);
			device_data->mCurThread = cur_thread;
		}

		inline void GraphicsContext::rasterizeLine(const ShaderVertexPhaseOutput& p1, const ShaderVertexPhaseOutput& p2)
		{
			auto device = m_contextData->mDevice;
			auto device_data = (detail::DeviceData*)device.handle();
			if (device_data->mNoBlock == false)
				device.waitDevice();
			auto pipeline_data = (detail::PipelineData*)m_contextData->mCurPipeline.handle();
			auto thread_count = device_data->mThreadPool.threadCount();
			auto cur_thread = device_data->mCurThread + 1;
			cur_thread %= thread_count;

			device_data->mThreadPool.push([this, p1, p2,
				pipeline = m_contextData->mCurPipeline,
				scissor = m_contextData->mCurScissor,
				fragment_shader = ((detail::ShaderData*)pipeline_data->mShader.handle())->mFragmentShader,
				resource_data = m_contextData->mTempRenderResources]() {//

				ShaderFragmentPhaseInput frag_input;
				ShaderFragmentPhaseOutput frag_output;
				int dx, dy, s1, s2, temp, interchange = 0, p, i;
				float x, y;
				float t;
				vec2 fp1, fp2;
				fp1 = p1.mPosition.xy;
				fp2 = p2.mPosition.xy;

				dx = math::abs(fp2.x - fp1.x);
				dy = math::abs(fp2.y - fp1.y);
				s1 = math::sign(fp2.x - fp1.x);
				s2 = math::sign(fp2.y - fp1.y); //direction
				x = fp1.x + 0.5f * s1;
				y = fp1.y + 0.5f * s2;
				if (dy > dx)
				{ //decide m value
					temp = dx;
					dx = dy;
					dy = temp; //dx changes fast
					interchange = 1;
				} //in 2,3,6,7 octant
				p = 2 * dy - dx; //initial value
				for (i = 0; i < dx; i++) {
					t = float(i) / float(dx);
					frag_input.mFragPos = ivec2(x, y);
					frag_input.mDepth = (1 - t) * p1.mPosition.z + t * p2.mPosition.z;
					frag_input.mStencil = 0;
					auto w = (1 - t) * p1.mPosition.w + t * p2.mPosition.w;
					w = math::inverse(w);
					for (int i = 0; i < detail::MaxShaderAttributeCount; i++)
						frag_input.mAttributes[i] = w * ((1 - t) * p1.mAttributes[i] + t * p2.mAttributes[i]);

					frag_output.mDiscard = false;
					fragment_shader(resource_data, frag_input, frag_output);
					if (frag_output.mDiscard == false)
						acceptFragment(pipeline, frag_output, frag_input.mFragPos, frag_input.mDepth, scissor);

					if (p > 0) {
						if (interchange)
							x = x + s1; /*x i as y i */
						else
							y = y + s2;
						p = p - 2 * dx; //p i+1 =p i +2*(△y -△x)
					}
					if (interchange) //if pi<=0，y i no change
						y = y + s2; //y i as x i
					else
						x = x + s1;
					p = p + 2 * dy;
				}

			}, cur_thread);//

			device_data->mCurThread = cur_thread;
		}

		inline void GraphicsContext::rasterizeTriganle(const ShaderVertexPhaseOutput& p1, const ShaderVertexPhaseOutput& p2, const ShaderVertexPhaseOutput& p3)
		{
			auto device = m_contextData->mDevice;
			auto device_data = (detail::DeviceData*)device.handle();
			if (device_data->mNoBlock == false)
				device.waitDevice();
			auto pipeline_data = (detail::PipelineData*)m_contextData->mCurPipeline.handle();
			auto thread_count = device_data->mThreadPool.threadCount();
			auto cur_thread = device_data->mCurThread + 1;
			cur_thread %= thread_count;

			vec2 pos[3] = {
				p1.mPosition.xy ,
				p2.mPosition.xy,
				p3.mPosition.xy };

			vec2 xymin = pos[0], xymax = pos[0];
			for (int i = 1; i < 3; i++)
			{
				if (pos[i].x < xymin.x) xymin.x = pos[i].x;
				if (pos[i].x > xymax.x) xymax.x = pos[i].x;
				if (pos[i].y < xymin.y) xymin.y = pos[i].y;
				if (pos[i].y > xymax.y) xymax.y = pos[i].y;
			}
			ivec2 ixymin = ivec2(math::round(xymin)), ixymax = ivec2(math::round(xymax));
			if (ixymin.x < 0) ixymin.x = 0;
			if (ixymin.y < 0) ixymin.y = 0;
			if (ixymax.x > m_contextData->mCurTargetSize.x) ixymax.x = m_contextData->mCurTargetSize.x;
			if (ixymax.y > m_contextData->mCurTargetSize.y) ixymax.y = m_contextData->mCurTargetSize.y;

			if (device_data->mNoBlock)
			{

				device_data->mThreadPool.push([this, p1, p2, p3, pos, ixymin, ixymax, thread_count,
					pipeline = m_contextData->mCurPipeline, scissor = m_contextData->mCurScissor,
					fragment_shader = ((detail::ShaderData*)pipeline_data->mShader.handle())->mFragmentShader,
					resource_data = m_contextData->mTempRenderResources]() {
					ShaderFragmentPhaseInput frag_input;
					ShaderFragmentPhaseOutput frag_output;
					vec2 cur_pos;
					math::vec2 v0, v1, v2;
					float dot00, dot01, dot02, dot11, dot12;
					double invDenom;
					double u, v;
					for (int y = ixymin.y; y < ixymax.y; y++)
					{
						/*if ((y) % thread_count == tid)*/
						{
							cur_pos.y = y + 0.5f;
							for (int x = ixymin.x; x < ixymax.x; x++)
							{
								cur_pos.x = x + 0.5f;
								v0 = math::vec2(pos[1] - pos[0]);
								v1 = math::vec2(pos[2] - pos[0]);
								v2 = math::vec2(cur_pos - pos[0]);
								// 计算点乘
								dot00 = math::dot(v0, v0);
								dot01 = math::dot(v0, v1);
								dot02 = math::dot(v0, v2);
								dot11 = math::dot(v1, v1);
								dot12 = math::dot(v1, v2);
								// 计算重心坐标系下的坐标
								invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
								u = (dot11 * dot02 - dot01 * dot12) * invDenom;
								v = (dot00 * dot12 - dot01 * dot02) * invDenom;
								if (u < 0 || v < 0 || (u + v) > 1)
									continue;

								frag_input.mFragPos = ivec2(cur_pos);
								frag_input.mDepth = (1 - u - v) * p1.mPosition.z + u * p2.mPosition.z + v * p3.mPosition.z;
								frag_input.mStencil = 0;
								auto w = (1 - u - v) * p1.mPosition.w + u * p2.mPosition.w + v * p3.mPosition.w;
								w = math::inverse(w);
								for (int i = 0; i < detail::MaxShaderAttributeCount; i++)
									frag_input.mAttributes[i] = w * ((1 - u - v) * p1.mAttributes[i] + u * p2.mAttributes[i] + v * p3.mAttributes[i]);

								frag_output.mDiscard = false;
								fragment_shader(resource_data, frag_input, frag_output);
								if (frag_output.mDiscard == false)
									acceptFragment(pipeline, frag_output, frag_input.mFragPos, frag_input.mDepth, scissor);
							}
						}

					}

				}, cur_thread);

			}
			else
			{
				for (int tid = 0; tid < thread_count; tid++)
				{
					device_data->mThreadPool.push([this, p1, p2, p3, pos, ixymin, ixymax, tid, thread_count,
						pipeline = m_contextData->mCurPipeline, scissor = m_contextData->mCurScissor,
						fragment_shader = ((detail::ShaderData*)pipeline_data->mShader.handle())->mFragmentShader,
						resource_data = m_contextData->mTempRenderResources]() {
						ShaderFragmentPhaseInput frag_input;
						ShaderFragmentPhaseOutput frag_output;
						vec2 cur_pos;
						math::vec2 v0, v1, v2;
						float dot00, dot01, dot02, dot11, dot12;
						double invDenom;
						double u, v;
						for (int y = ixymin.y; y < ixymax.y; y++)
						{
							if ((y) % thread_count == tid)
							{
								cur_pos.y = y + 0.5f;
								for (int x = ixymin.x; x < ixymax.x; x++)
								{
									cur_pos.x = x + 0.5f;
									v0 = math::vec2(pos[1] - pos[0]);
									v1 = math::vec2(pos[2] - pos[0]);
									v2 = math::vec2(cur_pos - pos[0]);
									// 计算点乘
									dot00 = math::dot(v0, v0);
									dot01 = math::dot(v0, v1);
									dot02 = math::dot(v0, v2);
									dot11 = math::dot(v1, v1);
									dot12 = math::dot(v1, v2);
									// 计算重心坐标系下的坐标
									invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
									u = (dot11 * dot02 - dot01 * dot12) * invDenom;
									v = (dot00 * dot12 - dot01 * dot02) * invDenom;
									if (u < 0 || v < 0 || (u + v) > 1)
										continue;

									frag_input.mFragPos = ivec2(cur_pos);
									frag_input.mDepth = (1 - u - v) * p1.mPosition.z + u * p2.mPosition.z + v * p3.mPosition.z;
									frag_input.mStencil = 0;
									auto w = (1 - u - v) * p1.mPosition.w + u * p2.mPosition.w + v * p3.mPosition.w;
									w = math::inverse(w);
									for (int i = 0; i < detail::MaxShaderAttributeCount; i++)
										frag_input.mAttributes[i] = w * ((1 - u - v) * p1.mAttributes[i] + u * p2.mAttributes[i] + v * p3.mAttributes[i]);

									frag_output.mDiscard = false;
									fragment_shader(resource_data, frag_input, frag_output);
									if (frag_output.mDiscard == false)
										acceptFragment(pipeline, frag_output, frag_input.mFragPos, frag_input.mDepth, scissor);
								}
							}

						}

					}, tid);
				}
			}


			device_data->mCurThread = cur_thread;
		}






	}
}