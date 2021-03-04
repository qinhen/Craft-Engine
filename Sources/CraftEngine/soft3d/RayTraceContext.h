#pragma once
#include "./Device.h"
#include "./RayTraceAccelerationStructure.h"
#include "./RayTraceShader.h"
#include "./RayTracePipeline.h"

namespace CraftEngine
{
	namespace soft3d
	{

	

		namespace detail
		{
			struct RayTraceContextData
			{
				Device mDevice;
				RayTracePipeline mCurPipeline;
				RayTraceShaderResources mTempResources;
				ivec2 mTileSize = ivec2(32, 32);
				ivec2 mTempNumTile;
				ivec2 mCurTileID;
				std::mutex mMutex;
			};
		}


		class RayTraceContext
		{
		private:
			detail::RayTraceContextData* m_contextData;
		public:
			RayTraceContext(void* handle) :m_contextData((detail::RayTraceContextData*)handle) { }
			RayTraceContext() :m_contextData(nullptr) { }
		public:
			void bindPipeline(const RayTracePipeline& pipeline)
			{
				m_contextData->mCurPipeline = pipeline;
			}
			void bindSampler(const Sampler& sampler, uint32_t index)
			{
				m_contextData->mTempResources.mSamplers[index] = sampler;
			}
			void bindImage(const Image& image, uint32_t index)
			{
				m_contextData->mTempResources.mImages[index] = image;
			}
			void bindBuffer(const Buffer& buffer, uint32_t index)
			{
				m_contextData->mTempResources.mBuffers[index] = buffer;
			}
			void bindUserData(const void* data, uint32_t index)
			{
				m_contextData->mTempResources.mUserDatas[index] = data;
			}
			void bindAccelerationStructure(const RayTraceTopLevelAccelerationStructure& topAS, uint32_t index)
			{
				m_contextData->mTempResources.mTopLevelAccelerationStructures[index] = topAS;
			}
		public:

			void waitDevice() const
			{
				auto device = m_contextData->mDevice;
				auto device_data = (detail::DeviceData*)device.handle();
				device_data->mThreadPool.wait();
			}

			bool isFinished() const
			{
				return m_contextData->mDevice.allFinished();
			}

			float getProgress() const
			{
				auto tile_id_x = m_contextData->mCurTileID.x;
				auto tile_id_y = m_contextData->mCurTileID.y;
				auto tile_id = tile_id_y * m_contextData->mTempNumTile.x + tile_id_x;				
				auto tile_count = m_contextData->mTempNumTile.x * m_contextData->mTempNumTile.y;
				return float(tile_id) / float(tile_count);
			}

			void traceRayTiled(int width, int height)
			{
				auto device = m_contextData->mDevice;
				auto device_data = (detail::DeviceData*)device.handle();
				auto thread_count = device_data->mThreadPool.threadCount();

				waitDevice();
				m_contextData->mTileSize = ivec2(32, 32);
				m_contextData->mTempNumTile = ivec2(ceil(vec2(width, height) / vec2(m_contextData->mTileSize)));
				m_contextData->mCurTileID = ivec2(0, 0);

				for (int tid = 0; tid < thread_count; tid++)
				{
					device_data->mThreadPool.push([thread_count, width, height, tid, this]() {
						RayTraceShaderRayGenPhaseInput input;
						RayTraceShaderPayload payload;
						int stack_buffer[128];
						RayTraceResult result;
						auto pipeline_data = (detail::RayTracePipelineData*)m_contextData->mCurPipeline.handle();
						auto shader_data = (detail::RayTraceShaderData*)pipeline_data->mShader.handle();
						auto resources = m_contextData->mTempResources;
						RayTraceCaller caller(&payload, shader_data, stack_buffer, &result, &resources);
						auto raygen_shader = shader_data->mRayGenShader;
						input.mLaunchSize = ivec2(width, height);

						//srand(time(0));
						while (1)
						{
							m_contextData->mMutex.lock();
							if (m_contextData->mCurTileID.x >= m_contextData->mTempNumTile.x)
							{
								m_contextData->mCurTileID.x = 0;
								if (m_contextData->mCurTileID.y >= m_contextData->mTempNumTile.y)
								{
									m_contextData->mMutex.unlock();
									return;
								}
								else
								{
									m_contextData->mCurTileID.y++;
								}
							}
							ivec2 tile_id = m_contextData->mCurTileID;
							m_contextData->mCurTileID.x++;
							m_contextData->mMutex.unlock();

							ivec2 tile_beg = tile_id * m_contextData->mTileSize;
							ivec2 tile_end = tile_beg + m_contextData->mTileSize;
							tile_end = math::clamp(tile_end, ivec2(0, 0), ivec2(width, height));
							for (int y = tile_beg.y; y < tile_end.y; y++)
							{
								for (int x = tile_beg.x; x < tile_end.x; x++)
								{
									input.mLaunchID = ivec2(x, y);
									raygen_shader(resources, payload, caller, input);
								}
							}
						}

					}, tid);
				}
			}

			void traceRay(int width, int height)
			{
				auto device = m_contextData->mDevice;
				auto device_data = (detail::DeviceData*)device.handle();
				auto thread_count = device_data->mThreadPool.threadCount();

				waitDevice();

				for (int tid = 0; tid < thread_count; tid++)
				{
					device_data->mThreadPool.push([thread_count, width, height, tid, this]() {
						RayTraceShaderRayGenPhaseInput input;
						RayTraceShaderPayload payload;
						int stack_buffer[128];
						RayTraceResult result;
						auto pipeline_data = (detail::RayTracePipelineData*)m_contextData->mCurPipeline.handle();
						auto shader_data = (detail::RayTraceShaderData*)pipeline_data->mShader.handle();
						auto resources = m_contextData->mTempResources;
						RayTraceCaller caller(&payload, shader_data, stack_buffer, &result, &resources);
						auto raygen_shader = shader_data->mRayGenShader;
						input.mLaunchSize = ivec2(width, height);

						{
							ivec2 tile_beg = ivec2(0, 0);
							ivec2 tile_end = ivec2(width, height);
							for (int y = tile_beg.y; y < tile_end.y; y++)
							{
								if ((y) % thread_count == tid)
									for (int x = tile_beg.x; x < tile_end.x; x++)
									{
										input.mLaunchID = ivec2(x, y);
										raygen_shader(resources, payload, caller, input);
									}
							}
						}

					}, tid);
				}
			}

			void* handle() const { return m_contextData; }
			bool  valid() const { return handle() != nullptr; }		

		};




		RayTraceContext createRayTrackContext(Device& device, const MemAllocator& allocator = MemAllocator())
		{
			auto ctx_data = (detail::RayTraceContextData*)allocator.alloc(sizeof(detail::RayTraceContextData));
			RayTraceContext context(ctx_data);
			ctx_data->mDevice = device;
			ctx_data->mTempNumTile = ivec2(0, 0);
			ctx_data->mCurTileID = ivec2(0, 0);
			new(&ctx_data->mMutex) std::mutex;
			return context;
		}

		void destroyRayTrackContext(RayTraceContext& context, const MemAllocator& allocator = MemAllocator())
		{
			auto ctx_data = (detail::RayTraceContextData*)allocator.alloc(sizeof(detail::RayTraceContextData));
			delete(&ctx_data->mMutex);
			allocator.free(context.handle());
		}




	}
}

