#pragma once
#include "./Common.h"

namespace CraftEngine
{
	namespace soft3d
	{

		namespace detail
		{
			struct MemoryData
			{
				void* mMemoryPtr;
				size_t mMemorySize;
			};
		}


		class Memory
		{
		private:
			detail::MemoryData* m_memoryData;
		public:
			explicit Memory(void* handle) : m_memoryData((detail::MemoryData*)handle) {}
			Memory() : m_memoryData(nullptr) {}
			size_t size() const { return m_memoryData->mMemorySize; }
			const void* data() const { return m_memoryData->mMemoryPtr; }
			void* data() { return m_memoryData->mMemoryPtr; }
			void* handle() const { return m_memoryData; }
			bool  valid() const { return handle() != nullptr; }
		};

		Memory createMemory(size_t size, const MemAllocator& allocator = MemAllocator())
		{
			auto mem_data = (detail::MemoryData*)allocator.alloc(sizeof(detail::MemoryData));
			mem_data->mMemoryPtr = allocator.alloc(size);
			mem_data->mMemorySize = size;
			return Memory(mem_data);
		}
		void destroyMemory(const Memory& memory, const MemAllocator& allocator = MemAllocator())
		{
			auto mem_data = (detail::MemoryData*)memory.handle();
			allocator.free(mem_data->mMemoryPtr);
			allocator.free(mem_data);
		}




	}
}