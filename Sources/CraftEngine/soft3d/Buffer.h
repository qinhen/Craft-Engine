#pragma once
#include "./Common.h"

namespace CraftEngine
{
	namespace soft3d
	{

		namespace detail
		{
			struct BufferData
			{
				Memory mMemory;
				size_t mMemoryOffset;
				size_t mBufferSize;
				void* mMappedPtr;
			};
		}

		class Buffer
		{
		private:
			detail::BufferData* m_bufferData = nullptr;
		public:
			explicit Buffer(void* handle) : m_bufferData((detail::BufferData*)handle) {}
			Buffer() : m_bufferData(nullptr) {}
			size_t size() const { return m_bufferData->mBufferSize; }
			size_t offset() const { return m_bufferData->mMemoryOffset; }
			const void* data() const { return m_bufferData->mMappedPtr; }
			void* data() { return m_bufferData->mMappedPtr; }

			void unbind()
			{
				m_bufferData->mMemoryOffset = 0;
				m_bufferData->mMemory = Memory(nullptr);
				m_bufferData->mMappedPtr = nullptr;
			}
			void bindMemory(const Memory& mem, size_t offset)
			{
				unbind();
				if (mem.valid())
				{
					if (offset + this->size() > mem.size())
						soft3d_throw_error(ErrorType::eBufferMemoryNotEnough);
					if (offset + this->size() > mem.size())
						return;
					m_bufferData->mMemoryOffset = offset;
					m_bufferData->mMemory = mem;
					m_bufferData->mMappedPtr = ((byte*)m_bufferData->mMemory.data()) + this->offset();
				}
			}
			void write(const void* data, size_t size, size_t offset)
			{
				if (offset > this->size() || offset + size > this->size())
					soft3d_throw_error(ErrorType::eBufferMemoryOutOfRange);

				if (offset >= this->size())
					return;
				if (offset + size > this->size())
					memcpy((byte*)this->data() + offset, data, this->size() - offset);
				else
					memcpy((byte*)this->data() + offset, data, size);
			}
			const Memory& memory() const { return m_bufferData->mMemory; }

			void* handle() const { return m_bufferData; }
			bool  valid() const { return handle() != nullptr; }
		};



		Buffer createBuffer(size_t size, const MemAllocator& allocator = MemAllocator())
		{
			auto buf_data = (detail::BufferData*)allocator.alloc(sizeof(detail::BufferData));
			buf_data->mBufferSize = size;
			buf_data->mMemoryOffset = 0;
			buf_data->mMemory = Memory(nullptr);
			buf_data->mMappedPtr = nullptr;
			return Buffer(buf_data);
		}
		void destroyBuffer(const Buffer& buffer, const MemAllocator& allocator = MemAllocator())
		{
			allocator.free(buffer.handle());
		}


	}
}