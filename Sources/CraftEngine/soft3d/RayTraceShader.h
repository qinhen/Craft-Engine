#pragma once

#include "./Memory.h"
#include "./Image.h"
#include "./Buffer.h"
#include "./Sampler.h"
#include "./RayTraceAccelerationStructure.h"

namespace CraftEngine
{
	namespace soft3d
	{


	
		namespace detail
		{
			constexpr int MaxRayTraceShaderResourceImageCount = 32;
			constexpr int MaxRayTraceShaderResourceBufferCount = 16;
			constexpr int MaxRayTraceShaderResourceSamplerCount = 8;
			constexpr int MaxRayTraceShaderResourceUserDataCount = 4;
			constexpr int MaxRayTraceShaderResourceAccelerationStructureCount = 4;
			constexpr int MaxRayTraceShaderPayloadSize = 4096;

			constexpr int MaxRayTraceShaderTableSize = 32;
		}

		struct RayTraceShaderResources
		{
			Image mImages[detail::MaxRayTraceShaderResourceImageCount];
			Buffer mBuffers[detail::MaxRayTraceShaderResourceBufferCount];
			Sampler mSamplers[detail::MaxRayTraceShaderResourceSamplerCount];
			RayTraceTopLevelAccelerationStructure mTopLevelAccelerationStructures[detail::MaxRayTraceShaderResourceAccelerationStructureCount];
			void const* mUserDatas[detail::MaxRayTraceShaderResourceUserDataCount];
		};

		struct RayTraceShaderRayGenPhaseInput
		{
			ivec2 mLaunchID;
			ivec2 mLaunchSize;
		};


		struct RayTraceShaderPayload
		{
			uint8_t mPayload[detail::MaxRayTraceShaderPayloadSize];
		};

		class RayTraceCaller;

		using RayTraceShaderRayGenPhaseFunc = void(*)(const RayTraceShaderResources&, RayTraceShaderPayload&, const RayTraceCaller&, const RayTraceShaderRayGenPhaseInput&);
		using RayTraceShaderRayClosestHitPhaseFunc = void(*)(const RayTraceShaderResources&, RayTraceShaderPayload&, const RayTraceCaller&, const RayTraceResult&);
		using RayTraceShaderRayMissPhaseFunc = void(*)(const RayTraceShaderResources&, RayTraceShaderPayload&, const RayTraceCaller&, const RayTraceResult&);


		namespace detail
		{
			struct RayTraceShaderData
			{
				RayTraceShaderRayGenPhaseFunc        mRayGenShader;
				RayTraceShaderRayClosestHitPhaseFunc mRayClosestHitShaders[detail::MaxRayTraceShaderTableSize];
				RayTraceShaderRayMissPhaseFunc       mRayMissShader;
			};



			void DefaultRayGenShader(const RayTraceShaderResources& resources, RayTraceShaderPayload& payload, const RayTraceCaller& caller, const RayTraceShaderRayGenPhaseInput& input)
			{
				
			};

			void DefaultRayClosestHitShader(const RayTraceShaderResources& resources, RayTraceShaderPayload& payload, const RayTraceCaller& caller, const RayTraceResult&)
			{
				
			};

			void DefaultRayMissShader(const RayTraceShaderResources& resources, RayTraceShaderPayload& payload, const RayTraceCaller& caller, const RayTraceResult&)
			{
				
			};
		}

		class RayTraceCaller
		{
		private:
			RayTraceShaderPayload* mPayload;
			detail::RayTraceShaderData* mShaderData;
			int* mStackBuffer;
			RayTraceResult* mResult;
			const RayTraceShaderResources* mResources;
		public:
			RayTraceCaller(void* handle0, void* handle1, void* handle2, void* handle3, const void* handle4)
			{
				mPayload = (RayTraceShaderPayload*)handle0;
				mShaderData = (detail::RayTraceShaderData*)handle1;
				mStackBuffer = (int*)handle2;
				mResult = (RayTraceResult*)handle3;
				mResources = (const RayTraceShaderResources*)handle4;
			}

			//void setSeed(const vec2& seed)
			//{
			//	*mSeed = seed;
			//}

			float rand() const
			{
				return ::rand() / float(RAND_MAX);
				//*mSeed -= mRandomVector;
				//return math::fract(math::sin(dot(*mSeed, vec2(12.9898, 78.233))) * 43758.5453);
			}

			float traceRay(const RayTraceTopLevelAccelerationStructure& acStructure, vec3 origin, vec3 direction, float tmin, float tmax) const
			{		
				detail::rayTraceRayHitTopLevelAccelerationStructure(acStructure, origin, direction, tmin, tmax, mStackBuffer, *mResult);
				if (mResult->rt_RayT >= 0.0f)
					mShaderData->mRayClosestHitShaders[mResult->rt_ShaderIndex](*mResources, *mPayload, *this, *mResult);
				else
					mShaderData->mRayMissShader(*mResources, *mPayload, *this, *mResult);
				return mResult->rt_RayT;
			}

			float traceRay(const RayTraceTopLevelAccelerationStructure& acStructure, vec3 origin, vec3 direction, float tmin, float tmax, RayTraceResult& result) const
			{
				detail::rayTraceRayHitTopLevelAccelerationStructure(acStructure, origin, direction, tmin, tmax, mStackBuffer, result);
				if (mResult->rt_RayT >= 0.0f)
					mShaderData->mRayClosestHitShaders[mResult->rt_ShaderIndex](*mResources, *mPayload, *this, result);
				else
					mShaderData->mRayMissShader(*mResources, *mPayload, *this, result);
				return result.rt_RayT;
			}

			bool anyHit(const RayTraceTopLevelAccelerationStructure& acStructure, vec3 origin, vec3 direction, float tmin, float tmax) const
			{
				return detail::rayTraceRayAnyHitTopLevelAccelerationStructure(acStructure, origin, direction, tmin, tmax, mStackBuffer);
			}

			void imageStore(const Image& image, ivec2 coord, vec4 color) const
			{
				auto img_data = (detail::ImageData*)image.handle();
				auto pixel_size = img_data->mPixelBytes;
				auto dst_pixel_data_begin = ((byte*)img_data->mMappedPtr) + (coord.y * image.width() + coord.x) * pixel_size;
				detail::ImagePixelData pixel_data = detail::castVectorToPixel(color, image.format());
				memcpy(dst_pixel_data_begin, &pixel_data, pixel_size);
			}

			vec4 imageRead(const Image& image, ivec2 coord) const
			{
				auto img_data = (detail::ImageData*)image.handle();
				auto pixel_size = img_data->mPixelBytes;
				auto dst_pixel_data_begin = ((byte*)img_data->mMappedPtr) + (coord.y * image.width() + coord.x) * pixel_size;
				vec4 color = detail::castPixelToVector(*((detail::ImagePixelData*)dst_pixel_data_begin), image.format());
				return color;
			}
		};



		class RayTraceShader
		{
		private:
			detail::RayTraceShaderData* m_shaderData;
		public:
			RayTraceShader(void* obj) : m_shaderData((detail::RayTraceShaderData*)obj) {}
			RayTraceShader() : m_shaderData(nullptr) {}
			void* handle() const { return m_shaderData; }
			bool  valid() const { return handle() != nullptr; }
		};

		RayTraceShader createRayTraceShader(
			RayTraceShaderRayGenPhaseFunc rayGenShader, 
			RayTraceShaderRayClosestHitPhaseFunc* rayClosestHitShaders, 
			uint32_t rayClosestHitShaderCount,
			RayTraceShaderRayMissPhaseFunc rayMissShader, 
			const MemAllocator& allocator = MemAllocator()
		){
			auto shader_data = (detail::RayTraceShaderData*)allocator.alloc(sizeof(detail::RayTraceShaderData));
			shader_data->mRayGenShader = rayGenShader;
			if (rayGenShader == nullptr)
				shader_data->mRayGenShader = detail::DefaultRayGenShader;
			for (int i = 0; i < rayClosestHitShaderCount; i++)
			{
				shader_data->mRayClosestHitShaders[i] = rayClosestHitShaders[i];
				if (rayClosestHitShaders[i] == nullptr)
					shader_data->mRayClosestHitShaders[i] = detail::DefaultRayClosestHitShader;
			}
			shader_data->mRayMissShader = rayMissShader;
			if (rayMissShader == nullptr)
				shader_data->mRayMissShader = detail::DefaultRayMissShader;
			return RayTraceShader(shader_data);
		}
		void destroyRayTraceShader(const RayTraceShader& shader, const MemAllocator& allocator = MemAllocator())
		{
			allocator.free(shader.handle());
		}




	}
}

