#pragma once
#include "./Buffer.h"

namespace CraftEngine
{
	namespace soft3d
	{


		size_t bufRead(
			void* dstAddress,
			const Buffer& srcBuffer,
			size_t count = ~0x0,
			size_t offset = 0
		);

		size_t bufWrite(
			Buffer& dstBuffer,
			void* srcAddress,
			size_t count,
			size_t offset = 0
		);

		size_t bufCopy(
			Buffer& dstBuffer,
			const Buffer& srcBuffer,
			size_t count,
			size_t dstOffset = 0,
			size_t srcOffset = 0
		);

	}
}


namespace CraftEngine
{
	namespace soft3d
	{

		size_t bufRead(
			void* dstAddress,
			const Buffer& srcBuffer,
			size_t count,
			size_t offset
		)
		{
			if (offset > srcBuffer.size())
				return 0;
			auto clamp_count = offset + count > srcBuffer.size() ? srcBuffer.size() - offset : count;
			memcpy(dstAddress, (const byte*)srcBuffer.data() + offset, count);
			return clamp_count;
		}

		size_t bufWrite(
			Buffer& dstBuffer,
			void* srcAddress,
			size_t count,
			size_t offset
		)
		{
			if (offset > dstBuffer.size())
				return 0;
			auto clamp_count = offset + count > dstBuffer.size() ? dstBuffer.size() - offset : count;
			memcpy((byte*)dstBuffer.data() + offset, srcAddress, count);
			return clamp_count;
		}

		size_t bufCopy(
			Buffer& dstBuffer,
			const Buffer& srcBuffer,
			size_t count,
			size_t dstOffset,
			size_t srcOffset
		) {
			if (dstOffset > dstBuffer.size())
				return 0;
			if (srcOffset > srcBuffer.size())
				return 0;
			auto clamp_count = count;
			clamp_count = dstOffset + clamp_count > dstBuffer.size() ? dstBuffer.size() - dstOffset : clamp_count;
			clamp_count = srcOffset + clamp_count > srcBuffer.size() ? srcBuffer.size() - srcOffset : clamp_count;
			memcpy((byte*)dstBuffer.data() + dstOffset, (byte*)srcBuffer.data() + srcOffset, count);
			return clamp_count;
		}


	}
}


