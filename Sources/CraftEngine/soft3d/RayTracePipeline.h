#pragma once
#include "RayTraceShader.h"

namespace CraftEngine
{
	namespace soft3d
	{

	

		namespace detail
		{
			struct RayTracePipelineData
			{
				RayTraceShader mShader;
			};
		}


		class RayTracePipeline
		{
		private:
			detail::RayTracePipelineData* m_rayTracePipelineData;
		public:
			RayTracePipeline(void* handle) :m_rayTracePipelineData((detail::RayTracePipelineData*)handle) { }
			RayTracePipeline() :m_rayTracePipelineData(nullptr) { }
		public:

			void* handle() const { return m_rayTracePipelineData; }
			bool  valid() const { return handle() != nullptr; }		
		};




		RayTracePipeline createRayTracePipeline(const RayTraceShader& shader)
		{
			auto pipeline_data = new detail::RayTracePipelineData;
			RayTracePipeline pipeline(pipeline_data);
			pipeline_data->mShader = shader;
			return pipeline;
		}

		void destroyRayTracePipeline(RayTracePipeline& pipeline, const MemAllocator& allocator = MemAllocator())
		{
			auto pipeline_data = (detail::RayTracePipelineData*)pipeline.handle();
			delete pipeline_data;
		}




	}
}

