#pragma once
#include "../math/LinearMath.h"
#include "../core/Core.h"
#include "../core/Bitmap.h"

//#define craft_engine_soft3d_error assert(false)
//#define craft_engine_soft3d_assert assert

#define soft3d_throw_error(error) \
	throw error

#define soft3d_assert_error(expr ,error) \
	if(!(expr)) throw error

namespace CraftEngine
{
	namespace soft3d
	{

		using byte = uint8_t;
		using vec2 = math::vec2;
		using vec3 = math::vec3;
		using vec4 = math::vec4;
		using ivec2 = math::ivec2;
		using ivec3 = math::ivec3;
		using ivec4 = math::ivec4;
		using u8vec4 = math::Vector<4, uint8_t>;

		enum class ErrorType
		{
			eInvalidEnum,
			eInvalidBuffer,
			eBufferMemoryNotEnough,
			eBufferMemoryOutOfRange,
			eInvalidImage,
			eImageMemoryNotEnough,
			eWrongImageType,
			eInvalidParam,
			eFrameBufferColorTargetOutOfRange,
		};


		namespace detail
		{
			using MemoryAllocatorAllocFuncType = void* (size_t);
			using MemoryAllocatorFreeFuncType = void (void*);

			struct MemoryAllocatorData
			{
				MemoryAllocatorAllocFuncType* mAllocFunc;
				MemoryAllocatorFreeFuncType* mFreeFunc;
			};
		}
		using MemoryAllocatorAllocFunc = detail::MemoryAllocatorAllocFuncType*;
		using MemoryAllocatorFreeFunc = detail::MemoryAllocatorFreeFuncType*;


		//namespace detail
		//{
		//	struct ObjectData
		//	{
		//		int32_t mObjectType;
		//	};
		//}

		//class Object
		//{
		//private:
		//	detail::ObjectData* m_objectData;
		//};

		enum class IndexType
		{
			eUInt32,
			eUInt16,
			eUInt8,
		};

		class MemAllocator
		{
		private:
			detail::MemoryAllocatorData* m_memoryAllocatorData;
		public:
			explicit MemAllocator(void* handle) :m_memoryAllocatorData((detail::MemoryAllocatorData*)handle) {}
			MemAllocator() :m_memoryAllocatorData(nullptr) {}
			inline void* alloc(size_t size) const
			{ 
				if (valid())
					return m_memoryAllocatorData->mAllocFunc(size);
				return ::malloc(size);
			}
			inline void free(void* ptr) const
			{
				if (valid())
					return m_memoryAllocatorData->mFreeFunc(ptr);
				::free(ptr);
			}
			void* handle() const { return m_memoryAllocatorData; }
			bool  valid() const { return handle() != nullptr; }
		};

		MemAllocator createMemoryAllocator(MemoryAllocatorAllocFunc allocFunc = nullptr, MemoryAllocatorFreeFunc freeFunc = nullptr)
		{
			auto alc_data = new detail::MemoryAllocatorData;
			if (allocFunc == nullptr || freeFunc == nullptr)
			{
				alc_data->mAllocFunc = ::malloc;
				alc_data->mFreeFunc = ::free;
			}
			else
			{
				alc_data->mAllocFunc = allocFunc;
				alc_data->mFreeFunc = freeFunc;
			}
			return MemAllocator(alc_data);
		}
		void destroyMemoryAllocator(const MemAllocator& allocator)
		{
			auto alc_data = (detail::MemoryAllocatorData*)allocator.handle();
			delete alc_data;
		}

	}
}