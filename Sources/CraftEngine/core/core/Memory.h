#pragma once
#include "../Common.h"

namespace CraftEngine
{
	namespace core
	{

		// 内存分配器
		template<typename Type>
		class MemoryAllocator {
		private:
			typedef MemoryAllocator<Type> this_type;

			void* memoryAllocate(size_t byteCount) { return new char[byteCount]; }
			void  memoryFree(void* memPtr) { delete[] memPtr; }
		public:
			typedef Type* pointer;
			typedef Type& reference;
			typedef const Type* const_pointer;
			typedef const Type& const_reference;

			MemoryAllocator()
			{

			}

			pointer address(reference ref) const { return &ref; }

			const_pointer address(const_reference ref) const { return &ref; }

			pointer allocate(uint32_t count)
			{
				return reinterpret_cast<pointer>(memoryAllocate(sizeof(Type) * count));
			}

			void construct(pointer ptr, const Type& value)
			{
				new(ptr) Type(value);
			}

			void deallocate(pointer ptr)
			{
				memoryFree(reinterpret_cast<void*>(ptr));
			}

			void destroy(pointer ptr)
			{
				ptr->~Type();
			}
		};



		// 内存分配器
		template<typename Type, uint32_t Alignment = 16>
		class AlignedMemoryAllocator {
		private:
			typedef AlignedMemoryAllocator<Type, Alignment> this_type;

			void* memoryAllocate(size_t byteCount) { return new char[byteCount]; }

			void  memoryFree(void* memPtr) { delete[] memPtr; }

			void* alignedMemoryAllocate(size_t byteCount, unsigned alignment)
			{
				void* ret = nullptr;
				char* allMem = (char*)memoryAllocate(byteCount + sizeof(void*) + (alignment - 1));
				if (allMem) {
					ret = alignPointer(allMem + sizeof(void*), alignment);
					*((void**)(ret)-1) = (void*)(allMem);
				}
				else {
					ret = (void*)allMem;
				}
				return ret;
			}

			void alignedMemoryFree(void* memPtr)
			{
				if (memPtr)
					memoryFree(*((void**)(memPtr)-1));
			}

			void* alignPointer(void* unalignedPtr, size_t alignment)
			{
				const size_t bit_mask = ~(alignment - 1);
				void* address = unalignedPtr;
				*(size_t*)(&address) += alignment - 1;
				*(size_t*)(&address) &= bit_mask;
				return address;
			}

		public:
			typedef Type* pointer;
			typedef Type& reference;
			typedef const Type* const_pointer;
			typedef const Type& const_reference;

			AlignedMemoryAllocator()
			{
				static_assert(
					(Alignment | 0x4) == 0x4 ||
					(Alignment | 0x8) == 0x8 ||
					(Alignment | 0x10) == 0x10 ||
					(Alignment | 0x20) == 0x20 ||
					(Alignment | 0x40) == 0x40,
					"The template pamram \'Alignment\' must be a power of \'2\'!");
			}

			pointer address(reference ref) const { return &ref; }

			const_pointer address(const_reference ref) const { return &ref; }

			pointer allocate(uint32_t count)
			{
				return reinterpret_cast<pointer>(alignedMemoryAllocate(sizeof(Type) * count, static_cast<unsigned>(Alignment)));
			}

			void construct(pointer ptr, const Type& value)
			{
				new(ptr) Type(value);
			}

			void deallocate(pointer ptr)
			{
				alignedMemoryFree(reinterpret_cast<void*>(ptr));
			}

			void destroy(pointer ptr)
			{
				ptr->~Type();
			}
		};




	}
}