#pragma once
#include "./Common.h"
#include "./Shader.h"
namespace CraftEngine
{
	namespace soft3d
	{



		enum class PipelineFrontFace
		{
			eClockWire,
			eCounterClockWire,
		};
		enum class PipelineCullFace
		{
			eCullNone,
			eCullFrontFace,
			eCullBackFace,
			eCullFrontAndBackFace,
		};
		enum class PipelinePolygonType
		{
			ePointList,
			eLineList,
			eLineStrip,
			eTriangleList,
			eTriangleStrip,
			eTriangleFan,
			ePolygon,
		};
		enum class PipelinePolygonMode
		{
			eFillMode,
			eLineMode,
			ePointMode,
		};
		enum class PipelineDepthCompareMode
		{
			eLessMode,
			eLessEqualMode,
			eGreaterMode,
			eGreaterEqualMode,
			eEqualMode,
		};



		namespace detail
		{
			struct PipelineData
			{
				// 
				Shader   mShader;
				uint32_t mAttachmentCount;

				// vertex-input
				size_t mVertexStride;

				// rasterizer-states
				PipelineFrontFace mFrontFace;
				PipelineCullFace mCullFace;
				bool mEnableCullFace;
				PipelinePolygonType mPolygonType;
				PipelinePolygonMode mPolygonMode;
				float mLineWidth;
				float mPointSize;

				// depth-stencil
				PipelineDepthCompareMode mDepthCompareMode;
				bool mEnableDepthTest;
				bool mEnableDepthWrite;
				bool mEnableStencilTest;
				bool mEnableStencilWrite;

				// color-blend
				bool mEnableColorBlend;

				// mCurScissor
				bool mEnableScissorTest;
			};

		}


		class Pipeline
		{
		private:
			detail::PipelineData* m_pipelineData;
		public:
			Pipeline(void* obj) : m_pipelineData((detail::PipelineData*)obj) {}
			Pipeline() : m_pipelineData(nullptr) {}

			void enableDepthTest(bool enable)
			{
				m_pipelineData->mEnableDepthTest = enable;
			}
			void enableDepthWrite(bool enable)
			{
				m_pipelineData->mEnableDepthWrite = enable;
			}
			void enableCullFace(bool enable)
			{
				m_pipelineData->mEnableCullFace = enable;
			}
			void enableStentilTest(bool enable)
			{
				m_pipelineData->mEnableStencilTest = enable;
			}
			void enableStencilWrite(bool enable)
			{
				m_pipelineData->mEnableStencilWrite = enable;
			}
			void enableColorBlend(bool enable)
			{
				m_pipelineData->mEnableColorBlend = enable;
			}
			void enableScissorTest(bool enable)
			{
				m_pipelineData->mEnableScissorTest = enable;
			}
			void* handle() const { return m_pipelineData; }
			bool  valid() const { return handle() != nullptr; }
		};


		Pipeline createPipeline(
			Shader shader,
			size_t vertexStride,
			uint32_t attachmentCount = 1,
			PipelineFrontFace frontFace = PipelineFrontFace::eClockWire,
			PipelineCullFace cullFace = PipelineCullFace::eCullNone,
			PipelinePolygonType polygonType = PipelinePolygonType::eTriangleList,
			PipelinePolygonMode polygonMode = PipelinePolygonMode::eFillMode,
			PipelineDepthCompareMode depthCompareMode = PipelineDepthCompareMode::eLessEqualMode,
			const MemAllocator& allocator = MemAllocator()
		)
		{
			auto pipeline_data = (detail::PipelineData*)allocator.alloc(sizeof(detail::PipelineData));
			pipeline_data->mShader = shader;
			pipeline_data->mVertexStride = vertexStride;
			pipeline_data->mAttachmentCount = attachmentCount;
			pipeline_data->mFrontFace = frontFace;
			pipeline_data->mCullFace = cullFace;
			pipeline_data->mEnableCullFace = false;
			pipeline_data->mPolygonType = polygonType;
			pipeline_data->mPolygonMode = polygonMode;
			pipeline_data->mLineWidth = 1.0f;
			pipeline_data->mPointSize = 1.0f;
			pipeline_data->mDepthCompareMode = depthCompareMode;
			pipeline_data->mEnableDepthTest = false;
			pipeline_data->mEnableStencilTest = false;
			pipeline_data->mEnableColorBlend = false;
			pipeline_data->mEnableDepthWrite = true;
			pipeline_data->mEnableStencilWrite = true;
			pipeline_data->mEnableScissorTest = false;
			return Pipeline(pipeline_data);
		}
		void destroyPipeline(const Pipeline& pipeline, const MemAllocator& allocator = MemAllocator())
		{
			allocator.free(pipeline.handle());
		}

	}
}